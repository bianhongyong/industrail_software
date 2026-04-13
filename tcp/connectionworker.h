#ifndef CONNECTIONWORKER_H
#define CONNECTIONWORKER_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>
#include <QImage>
#include <QDateTime>
#include <QBuffer>
#include <QQueue>
#include <QTimer>
#include <QElapsedTimer>
#include <QObject>
#include <QImageWriter>
// 任务状态枚举
enum class TaskStatus{
    IDLE,           // 空闲，可处理新任务
    CONNECTING,     // 正在连接服务器
    BUSY,           // 正在处理任务
    ERROR,          // 连接错误
    UNKNOWN         // 未知状态
};
enum Send_type{
    SEND_IMAGE=0,     // 发送图像
    SEND_BYTE       // 发送字节数据
};
// 发送任务结构体
struct SendTask {
    QImage image;            // 要发送的图像
    QByteArray data;         // 要发送的数据
    QString imageInfo;       // 图像信息
    int timeoutMs;           // 超时时间
    int sent_byte;           // 已发送字节数
    int total_byte;          // 总共要发送的字节数
    bool completed;          // 任务是否完成
    bool success;            // 任务是否成功
    Send_type send_type;     // 发送类型
    QByteArray responseData; // 响应数据

    SendTask() : timeoutMs(5000), completed(false), success(false),sent_byte(0) {}
};

class ConnectionWorker : public QThread
{
    Q_OBJECT
public:
    ConnectionWorker(const QString &host, int port, QObject *parent = nullptr);
    ~ConnectionWorker();

    // 发送数据
    bool sendImage(const QImage &image, const QString &imageInfo, int timeoutMs = 10000);
    bool sendByte(const QByteArray &data, const QString &info, int timeoutMs = 10000);
    // 获取响应数据
    QByteArray getResponse();

    // 获取当前任务状态
    TaskStatus status();

    // 重置连接
    bool resetConnection();

    // 断开连接
    void disconnect();

    // 获取统计信息
    int totalSent();
    int successCount();
    int errorCount();
    qint64 bytesTransferred();

    // 检查连接是否仍然有效
    bool isValid();

    // 设置连接超时时间
    void setConnectionTimeout(int timeoutMs);

    // 获取服务器信息
    QString serverInfo();

    // 返回连接的主机和端口信息
    QString hostPort() const {
        return QString("%1:%2").arg(m_host).arg(m_port);
    }
    void wake_up() {
        m_waitCondition.wakeAll();
    }

signals:
    // 连接状态信号
    void connected();
    void disconnected();
    void errorOccurred(QAbstractSocket::SocketError error, const QString &errorString);

    // 任务相关信号
    void sendStarted(const QString &imageInfo);
    void sendCompleted(bool success, const QByteArray &response);
    void progressUpdated(qint64 bytesSent, qint64 bytesTotal);

protected:
    void run() override;

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onSocketReadyRead();
    void onSocketBytesWritten(qint64 bytes);
    void onConnectionTimeout();

private:
    // 连接信息
    QString m_host;
    int m_port;
    QTcpSocket *m_socket;

    // 同步对象
    QMutex m_mutex;
    QWaitCondition m_waitCondition;

    // 状态
    TaskStatus m_status;
    bool m_shouldQuit;

    // 当前任务
    SendTask m_currentTask;
    // 缓冲区
    QByteArray m_receiveBuffer;
    qint64 m_expectedDataSize;
    bool m_headerReceived;

    // 统计信息
    int m_totalSent;
    int m_successCount;
    int m_errorCount;
    qint64 m_bytesTransferred;

    // 超时控制
    QTimer *m_connectionTimer;
    QElapsedTimer m_sendTimer;
    int m_connectionTimeout;

    // 私有方法,只允许被run方法调用
    void connectToServer();//尝试连接到服务器
    bool sendData(const QByteArray &data);
    QByteArray createDataPacket(const QImage &image, const QString &info);
    void processResponse(const QByteArray &data);
    void setStatus(TaskStatus status);
    void resetInternalState();
    void logMessage(const QString &message, bool isError = false);
};



class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr) : QObject(parent) {
        m_socket = new QTcpSocket(this);
        connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    }

    void connectToHost(const QString &host, quint16 port) {
        m_socket->connectToHost(host, port);
    }

private slots:
    void onDisconnected() {
        qDebug() << "Disconnected from host";
        // 连接断开后的处理逻辑
    }

private:
    QTcpSocket *m_socket;
};

#endif // CONNECTIONWORKER_H
