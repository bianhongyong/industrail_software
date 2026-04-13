#ifndef IMAGESENDTEST_H
#define IMAGESENDTEST_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QStringList>
#include <QVector>
#include "tcp/tcpconnectionpool.h"

class ImageSendTest : public QObject
{
    Q_OBJECT

public:
    ImageSendTest(QObject *parent = nullptr);
    ~ImageSendTest();

    // 初始化测试类，加载指定文件夹中的图片
    bool initialize(const QString &imageFolderPath, TCPConnectionPool *pool);

    // 开始测试，指定发送间隔
    void startTest(int sendIntervalMs = 100);

    // 停止测试并输出结果
    void stopTest();

signals:
    // 测试完成信号
    void testCompleted(int totalSent, int successCount, int errorCount, double throughput);

private slots:
    // 发送下一张图片
    void sendNextImage();

    // 处理发送完成回调
    void onSendCompleted(bool success, const QString &imageName, qint64 elapsedMs);

private:
    // 异步发送单张图片
    void sendImageAsync(const QImage &image, const QString &imageInfo);

    // 打印测试结果
    void printTestResults();

private:
    QStringList m_imagePaths;       // 图片路径列表
    QTimer *m_timer;                // 发送定时器
    int m_sendInterval;             // 发送间隔(毫秒)
    int m_currentIndex;             // 当前图片索引
    int m_totalSent;                // 总发送数
    int m_successCount;             // 成功数
    int m_errorCount;               // 失败数
    QVector<qint64> m_sendTimes;    // 每张图片的发送耗时
    TCPConnectionPool *m_tcpPool;   // TCP连接池引用
    QElapsedTimer m_testTimer;      // 测试计时器
    bool m_testRunning;             // 测试运行状态
};

#endif // IMAGESENDTEST_H
