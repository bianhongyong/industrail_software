#include "label.h"
#include "collect.h"
#include <QApplication>
#include <QCameraInfo>
#include <QVideoWidget>
#include <tcp/tcpconnectionpool.h>
#include <tcp/tcp_test.h>
#include <QThread>
#include <QTimer>  // 添加 QTimer 头文件
#include <QObject>
class MyApplication : public QApplication {
public:
    using QApplication::QApplication;

    bool notify(QObject* receiver, QEvent* event) override {
        try {
            return QApplication::notify(receiver, event);
        } catch (const std::exception& e) {
            // 在这里处理异常
            qCritical() << "Exception caught:" << e.what();
            saveImportantData();  // 崩溃前保存数据
            QCoreApplication::exit(EXIT_FAILURE);
            // 可以在这里选择退出程序或尝试恢复
            return false;
        } catch (...) {
            qCritical() << "Unknown exception caught";
            saveImportantData();
            QCoreApplication::exit(EXIT_FAILURE);
            return false;
        }
    }
private:
    void saveImportantData() {
        Annotation_manager::save_json();
    }
};

// 简化版批量图片发送测试函数
void testBatchImageSend(TCPConnectionPool* pool, const QString& folderPath, int delayMs = 100)
{
    // 获取图片列表
    QDir dir(folderPath);
    QStringList filters;
    filters << "*.jpg";
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files, QDir::Name);

    if (fileList.isEmpty()) {
        qWarning() << QString::fromLocal8Bit("目录中没有找到PNG图片");
        return;
    }

    qInfo() << QString::fromLocal8Bit("找到 %1 张图片，准备发送...").arg(fileList.size());

    // 创建定时器发送图片
    QTimer* timer = new QTimer();

    QObject::connect(timer, &QTimer::timeout, [=]() mutable{
        // 检查是否所有图片都已发送
        static int currentIndex = 0;  // 使用静态变量
        if (currentIndex >= fileList.size()) {
            timer->stop();
            timer->deleteLater();
            qInfo() << QString::fromLocal8Bit("所有图片发送完成");
            return;
        }
        // 获取当前图片
        QFileInfo fileInfo = fileList.at(currentIndex);
        QString imagePath = fileInfo.absoluteFilePath();
        QString imageId = fileInfo.fileName();

        // 加载图片
        QImage image(imagePath);
        if (image.isNull()) {
            qWarning() << QString::fromLocal8Bit("无法加载图片:") << imagePath;
            currentIndex++;
            return;
        }

        // 获取连接并发送
        ConnectionWorker* connection = pool->getConnection(50);
        if (connection) {
            // 连接发送完成信号
            QObject::connect(connection, &ConnectionWorker::sendCompleted,
                            [=](bool success, const QByteArray&){
                // 释放连接
                pool->releaseConnection(connection);
            });

            // 发送图片
            bool initiated = connection->sendImage(image, imageId);
            if (!initiated) {
                qWarning() << QString::fromLocal8Bit("发送初始化失败:") << imageId;
                pool->releaseConnection(connection);
            }
        }
        else {
            qWarning() << QString::fromLocal8Bit("无法获取连接:") << imageId;
        }
        // 移动到下一张图片
        currentIndex++;
        qDebug() << QString::fromLocal8Bit("发送进度: %1/%2").arg(currentIndex).arg(fileList.size());
    });
    // 启动定时器，每隔指定时间发送一张
    timer->start(delayMs);
}

int main(int argc, char *argv[])
{
    //获取当前程序运行所在的文件夹路径
    QString appDirPath = QCoreApplication::applicationDirPath();
    //确保路径是绝对路径
    QDir dir(appDirPath);
    QString absolutePath = dir.absolutePath();
    configFilePath = absolutePath;
    MyApplication a(argc, argv);
    collect w;
    w.show();
    return a.exec();
}

/*
    获取当前程序运行所在的文件夹路径
    QString appDirPath = QCoreApplication::applicationDirPath();
    确保路径是绝对路径
    QDir dir(appDirPath);
    QString absolutePath = dir.absolutePath();
    configFilePath = absolutePath;
    MyApplication a(argc, argv);
    collect w;
    w.show();
    return a.exec();
*/

/*
    TCPConnectionPool* tcp_pool =TCPConnectionPool::getInstance();
    tcp_pool->initialize("127.0.0.1",55555,30,1000);
    
    // 设置 2 秒后执行测试
    QTimer::singleShot(2000, [=]() {
        // 图片目录路径
        QString imageFolderPath = "d://qt_apply//Mould_system//mould_system//images_example";
        qDebug() << QString::fromLocal8Bit("开始测试批量发送图片...");
        testBatchImageSend(tcp_pool, imageFolderPath, 50);
    });
*/

/*
    ConnectionWorker* worker = new ConnectionWorker("211.81.50.204",55555);
    worker->start();
    QTimer::singleShot(2000, [=]() {
        worker->sendImage(QImage("D://qt_apply//Mould_system//mould_system//images_example//20250321133235204.png"), "1.png");
    });
*/
