#include <tcp/tcp_test.h>
#include <QDir>
#include <QFileInfoList>
#include <QImage>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QMetaObject>
#include <algorithm>

ImageSendTest::ImageSendTest(QObject *parent)
    : QObject(parent),
      m_sendInterval(100),
      m_currentIndex(0),
      m_totalSent(0),
      m_successCount(0),
      m_errorCount(0),
      m_tcpPool(nullptr),
      m_testRunning(false)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ImageSendTest::sendNextImage);
}

ImageSendTest::~ImageSendTest()
{
    if (m_testRunning) {
        stopTest();
    }
}

bool ImageSendTest::initialize(const QString &imageFolderPath, TCPConnectionPool *pool)
{
    m_tcpPool = pool;
    if (!m_tcpPool) {
        qCritical() << QString::fromLocal8Bit("TCP连接池未初始化!");
        return false;
    }

    // 加载文件夹中的所有PNG图片
    QDir imageDir(imageFolderPath);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg";
    imageDir.setNameFilters(filters);

    QFileInfoList fileList = imageDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    if (fileList.isEmpty()) {
        qWarning() << QString::fromLocal8Bit("图片文件夹为空:") << imageFolderPath;
        return false;
    }

    // 保存图片文件路径
    for (const QFileInfo &fileInfo : fileList) {
        m_imagePaths.append(fileInfo.absoluteFilePath());
    }

    qInfo() << QString::fromLocal8Bit("已加载") << m_imagePaths.size()
            << QString::fromLocal8Bit("张图片,准备发送测试");
    return true;
}

void ImageSendTest::startTest(int sendIntervalMs)
{
    if(m_testRunning){
        qWarning() << QString::fromLocal8Bit("测试已在运行中!");
        return;
    }

    m_sendInterval = sendIntervalMs;
    m_currentIndex = 0;
    m_totalSent = 0;
    m_successCount = 0;
    m_errorCount = 0;
    m_sendTimes.clear();
    m_testRunning = true;

    // 记录开始时间
    m_testTimer.start();

    // 启动定时器,按指定间隔发送图片
    m_timer->start(m_sendInterval);

    qInfo() << QString::fromLocal8Bit("开始测试 - 发送间隔:") << m_sendInterval
            << QString::fromLocal8Bit("ms");
}

void ImageSendTest::stopTest()
{
    if (!m_testRunning) {
        return;
    }

    m_timer->stop();
    m_testRunning = false;

    // 输出测试结果
    printTestResults();
}

void ImageSendTest::sendNextImage()
{
    if (m_imagePaths.isEmpty()) {
        qWarning() << QString::fromLocal8Bit("没有图片可发送!");
        stopTest();
        return;
    }

    // 检查是否已发送完所有图片
    if (m_currentIndex >= m_imagePaths.size()) {
        qInfo() << QString::fromLocal8Bit("所有图片已发送完毕,停止测试");
        stopTest();
        return;
    }

    // 加载当前图片
    QString imagePath = m_imagePaths.at(m_currentIndex);
    QImage image(imagePath);
    if (image.isNull()) {
        qWarning() << QString::fromLocal8Bit("无法加载图片:") << imagePath;
        m_currentIndex++;
        m_errorCount++;
        return;
    }

    // 从文件名提取信息
    QFileInfo fileInfo(imagePath);
    QString imageInfo = fileInfo.fileName();
    // 异步发送图片
    sendImageAsync(image, imageInfo);

    // 移动到下一张图片
    m_currentIndex++;
    m_totalSent++;

    // 每10张图片输出一次状态
    if (m_totalSent % 10 == 0) {
        qInfo() << QString::fromLocal8Bit("已发送") << m_totalSent
                << QString::fromLocal8Bit("张图片,成功:") << m_successCount
                << QString::fromLocal8Bit(",失败:") << m_errorCount
                << QString::fromLocal8Bit(",剩余:") << (m_imagePaths.size() - m_currentIndex);
    }
}

void ImageSendTest::onSendCompleted(bool success, const QString &imageName, qint64 elapsedMs)
{
    if (success) {
        m_successCount++;
        m_sendTimes.append(elapsedMs);
        qDebug() << QString::fromLocal8Bit("图片发送成功:") << imageName
                << QString::fromLocal8Bit("耗时:") << elapsedMs
                << QString::fromLocal8Bit("ms");
    } else {
        m_errorCount++;
        qWarning() << QString::fromLocal8Bit("图片发送失败:") << imageName;
    }
}

void ImageSendTest::sendImageAsync(const QImage &image, const QString &imageInfo)
{
    // 获取一个连接
    ConnectionWorker* connection = m_tcpPool->getConnection(1000);
    if (!connection) {
        qWarning() << QString::fromLocal8Bit("无法获取TCP连接,发送失败:") << imageInfo;
        m_errorCount++;
        return;
    }
    // 记录发送开始时间
    QElapsedTimer sendTimer;
    sendTimer.start();

    // 发送图片
    bool sendInitiated = connection->sendImage(image, imageInfo);

    if (!sendInitiated) {
        qWarning() << QString::fromLocal8Bit("图片发送失败(初始化阶段):") << imageInfo;
        m_tcpPool->releaseConnection(connection);
        m_errorCount++;
        return;
    }

    // 创建一个监控线程来等待发送完成并释放连接
    QThread* monitorThread = new QThread();
    QObject* monitor = new QObject();
    monitor->moveToThread(monitorThread);

    // 线程启动后执行的代码
    connect(monitorThread, &QThread::started, monitor, [=]() {
        // 等待发送完成或超时
        int timeoutMs = 5000; // 5秒超时
        QElapsedTimer timeoutTimer;
        timeoutTimer.start();

        while (connection->status() == TaskStatus::BUSY && timeoutTimer.elapsed() < timeoutMs) {
            QThread::msleep(10);
        }

        // 计算总耗时
        qint64 totalElapsed = sendTimer.elapsed();

        // 检查发送结果
        bool success = (connection->status() == TaskStatus::IDLE);

        // 释放连接
        m_tcpPool->releaseConnection(connection);

        // 发送结果信号
        QMetaObject::invokeMethod(this, "onSendCompleted",
                                  Qt::QueuedConnection,
                                  Q_ARG(bool, success),
                                  Q_ARG(QString, imageInfo),
                                  Q_ARG(qint64, totalElapsed));

        // 清理监控对象和线程
        monitorThread->quit();
    });

    // 设置线程和监控对象的自动清理
    connect(monitorThread, &QThread::finished, monitor, &QObject::deleteLater);
    connect(monitorThread, &QThread::finished, monitorThread, &QThread::deleteLater);

    // 启动监控线程
    monitorThread->start();
}

void ImageSendTest::printTestResults()
{
    qint64 totalTestTime = m_testTimer.elapsed();

    qInfo() << "\n========== " << QString::fromLocal8Bit("测试结果") << " ==========";
    qInfo() << QString::fromLocal8Bit("总测试时间:") << totalTestTime << QString::fromLocal8Bit("ms (")
            << (totalTestTime / 1000.0) << QString::fromLocal8Bit("秒)");
    qInfo() << QString::fromLocal8Bit("发送图片总数:") << m_totalSent;
    qInfo() << QString::fromLocal8Bit("成功数量:") << m_successCount;
    qInfo() << QString::fromLocal8Bit("失败数量:") << m_errorCount;
    qInfo() << QString::fromLocal8Bit("成功率:")
            << (m_totalSent > 0 ? (m_successCount * 100.0 / m_totalSent) : 0) << "%";

    // 计算吞吐量
    double throughput = (m_totalSent * 1000.0) / totalTestTime;
    qInfo() << QString::fromLocal8Bit("吞吐量:") << throughput << QString::fromLocal8Bit("张/秒");

    // 计算延迟统计
    if (!m_sendTimes.isEmpty()) {
        std::sort(m_sendTimes.begin(), m_sendTimes.end());
        qint64 minTime = m_sendTimes.first();
        qint64 maxTime = m_sendTimes.last();
        qint64 totalTime = 0;
        for (qint64 time : m_sendTimes) {
            totalTime += time;
        }
        qint64 avgTime = totalTime / m_sendTimes.size();
        qint64 medianTime = m_sendTimes[m_sendTimes.size() / 2];

        qInfo() << QString::fromLocal8Bit("最小响应时间:") << minTime << QString::fromLocal8Bit("ms");
        qInfo() << QString::fromLocal8Bit("最大响应时间:") << maxTime << QString::fromLocal8Bit("ms");
        qInfo() << QString::fromLocal8Bit("平均响应时间:") << avgTime << QString::fromLocal8Bit("ms");
        qInfo() << QString::fromLocal8Bit("中位数响应时间:") << medianTime << QString::fromLocal8Bit("ms");
    }

    // 连接池状态
    qInfo() << QString::fromLocal8Bit("连接池状态:");
    qInfo() << "- " << QString::fromLocal8Bit("总连接数:") << m_tcpPool->totalConnectionCount();
    qInfo() << "- " << QString::fromLocal8Bit("可用连接数:") << m_tcpPool->availableConnectionCount();

    qInfo() << "================================\n";
}
