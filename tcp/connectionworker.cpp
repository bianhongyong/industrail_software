#include <tcp/connectionworker.h>
#include <QDataStream>
#include <QBuffer>
#include <QCoreApplication>
#include <QHostAddress>
#include <QRandomGenerator>
#include <QThread>
#include <QDebug>
#include <QtEndian>

ConnectionWorker::ConnectionWorker(const QString &host, int port, QObject *parent)
    : QThread(parent),
      m_host(host),
      m_port(port),
      m_socket(nullptr),
      m_status(TaskStatus::UNKNOWN),
      m_shouldQuit(false),
      m_expectedDataSize(0),
      m_headerReceived(false),
      m_totalSent(0),
      m_successCount(0),
      m_errorCount(0),
      m_bytesTransferred(0),
      m_connectionTimer(new QTimer(this)),
      m_connectionTimeout(2000)  // 默认10秒超时
{

}

ConnectionWorker::~ConnectionWorker()
{

}

bool ConnectionWorker::sendImage(const QImage &image, const QString &imageInfo, int timeoutMs)
{
    if (image.isNull()) {
        logMessage(QString::fromLocal8Bit("尝试发送空图像"), true);
        return false;
    }

    QMutexLocker locker(&m_mutex);
    if (m_status == TaskStatus::BUSY){//这种情况就不应该出现，线程池管理问题
        logMessage(QString::fromLocal8Bit("工作线程忙碌中，无法处理新任务"), true);
        return false;
    }

    if (m_status == TaskStatus::ERROR || m_status == TaskStatus::UNKNOWN) {
        logMessage(QString::fromLocal8Bit("连接状态错误"), true);
        return false;
    }

    // 设置任务信息
    m_currentTask = SendTask();
    m_currentTask.image = image;
    m_currentTask.imageInfo = imageInfo;
    m_currentTask.timeoutMs = timeoutMs;
    m_currentTask.send_type = SEND_IMAGE;

    // 更新状态
    setStatus(TaskStatus::BUSY);
    // 唤醒工作线程
    m_waitCondition.wakeAll();
    return true;
}

bool ConnectionWorker::sendByte(const QByteArray &data, const QString &info, int timeoutMs)
{
    if (data.isNull()) {
        logMessage(QString::fromLocal8Bit("尝试发送空字节"), true);
        return false;
    }

    QMutexLocker locker(&m_mutex);
    if (m_status == TaskStatus::BUSY) {
        logMessage(QString::fromLocal8Bit("工作线程忙碌中，无法处理新任务"), true);
        return false;
    }

    if (m_status == TaskStatus::ERROR || m_status == TaskStatus::UNKNOWN) {
        logMessage(QString::fromLocal8Bit("连接状态错误，尝试重置连接"), true);
        if (!resetConnection()) {
            return false;
        }
    }
    
    // 设置任务信息
    m_currentTask = SendTask();
    m_currentTask.data = data;
    m_currentTask.timeoutMs = timeoutMs;
    m_currentTask.send_type = SEND_BYTE;
    // 更新状态
    setStatus(TaskStatus::BUSY);
    // 唤醒工作线程
    m_waitCondition.wakeAll();
    return true;
}

TaskStatus ConnectionWorker::status()
{
    QMutexLocker locker(&m_mutex);
    return m_status;
}

bool ConnectionWorker::resetConnection()
{
    return 0;
}

void ConnectionWorker::disconnect()//断开连接,关闭进程
{
    if(!m_socket) return;//已经断开连接，该线程已作废
    QMutexLocker locker(&m_mutex);
    m_shouldQuit = true;
    m_waitCondition.wakeAll();
}
void ConnectionWorker::run(){
    connectToServer();//尝试连接服务器，由于套接字不能跨线程访问，所以所有使用套接字的程序必须在run函数里执行。
    // 主循环
    while (!m_shouldQuit) {
        // 处理特殊状态
        if (m_status == TaskStatus::ERROR || m_status == TaskStatus::UNKNOWN) {//错误状态,不处理直接结束，交给线程池管理
            m_shouldQuit = true;
            break;
        }
        // 在IDLE状态下等待任务
        if (m_status == TaskStatus::IDLE){
            if (m_shouldQuit) break;
            // 如果没有任务，继续等待
            if (m_status != TaskStatus::BUSY) {
                continue;
            }
        }

        // 处理发送任务
        if (m_status == TaskStatus::BUSY) {
            QMutexLocker locker(&m_mutex);
            // 获取任务信息
            QImage image = m_currentTask.image;
            QByteArray directData = m_currentTask.data;
            QString imageInfo = m_currentTask.imageInfo;

           // 创建数据包
           QByteArray data;
           if (m_currentTask.send_type == SEND_BYTE) {
               data = directData;
           } 
           else {
               data = createDataPacket(image, imageInfo);
           }

            //标记开始发送
            emit sendStarted(imageInfo);
            bool sendSuccess = sendData(data);
            if (!sendSuccess) {
                // 发送失败，更新任务状态
                m_currentTask.completed = true;
                m_currentTask.success = false;
                m_errorCount++;
                emit sendCompleted(false, QByteArray());//发送失败信号
                break;
            }
            // 等待响应 - 简化版本，仅发送数据不等待响应
            m_currentTask.completed = true;
            m_currentTask.success = true;

            // 更新统计信息
            m_totalSent++;
            m_successCount++;

            // 发送完成信号
            emit sendCompleted(true, QByteArray());

            // 重置为空闲状态
            setStatus(TaskStatus::IDLE);
        }
    }

    // 线程退出前清理
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->abort();
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    if (m_connectionTimer) {
        delete m_connectionTimer;
        m_connectionTimer = nullptr;
    }
    qDebug() << QString::fromLocal8Bit("工作线程已结束");
}

void ConnectionWorker::onSocketConnected()
{
    logMessage(QString::fromLocal8Bit("连接成功"));
}

void ConnectionWorker::onSocketDisconnected()
{
    logMessage(QString::fromLocal8Bit("断开连接"));
}

void ConnectionWorker::onSocketError(QAbstractSocket::SocketError error)
{

}

void ConnectionWorker::onSocketReadyRead()
{

}

void ConnectionWorker::onSocketBytesWritten(qint64 bytes)
{
    qDebug()<<QString::fromLocal8Bit("已发送 %1 字节").arg(bytes);
}

void ConnectionWorker::onConnectionTimeout()
{

}

void ConnectionWorker::connectToServer()
{
    if (m_socket) {
        m_socket->abort();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
    //创建连接
    m_socket = new QTcpSocket();

    connect(m_socket, &QTcpSocket::connected,this,&ConnectionWorker::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ConnectionWorker::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ConnectionWorker::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::bytesWritten, this, &ConnectionWorker::onSocketBytesWritten);
    // connect(m_socket, &QTcpSocket::bytesWritten, [&](qint64 bytes){//信号与槽
    //     qDebug()<<QString::fromLocal8Bit("已发送 %1 字节").arg(bytes);
    //     //m_currentTask.sent_byte += bytes;
    // });
    m_socket->connectToHost(m_host,m_port);
    bool success = m_socket->waitForConnected(1000);//等待连接
    if(success){
        setStatus(TaskStatus::IDLE);
    }
    else{
        setStatus(TaskStatus::ERROR);
    }
}

bool ConnectionWorker::sendData(const QByteArray &data)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState){
        logMessage(QString::fromLocal8Bit("套接字未连接，无法发送数据"), true);
        return false;
    }
    logMessage(QString::fromLocal8Bit("开始发送数据..."));
    // 发送数据
    qint64 bytesWritten = m_socket->write(data);
    if (bytesWritten == -1) {
        logMessage(QString::fromLocal8Bit("写入数据失败: %1").arg(m_socket->errorString()), true);
        return false;
    }
    m_socket->waitForBytesWritten();

      // 等待对方的确认消息
    if (!m_socket->waitForReadyRead()) {
        logMessage(QString::fromLocal8Bit("未收到确认消息"), true);
        return false;
    }
    QByteArray ack = m_socket->readAll();
    if (ack == "ACK") {
        logMessage(QString::fromLocal8Bit("数据已被对方接收"));
        return true;
    } else {
        logMessage(QString::fromLocal8Bit("收到未知响应: %1").arg(QString(ack)), true);
        return false;
    }
    return true;
}

QByteArray ConnectionWorker::createDataPacket(const QImage &image, const QString &imageInfo)
{
    // 将图像转换为高质量无损字节数组
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);

    // 使用最高质量的无损格式
    QImageWriter writer(&buffer, "PNG");
    writer.setQuality(100);  // 使用最高质量设置
    writer.setCompression(0); // 使用最低压缩率（最高质量）

    // 写入图像
    writer.write(image);
    buffer.close();

    // 创建标准化的数据包
    QByteArray packet;

    // 1. 添加起始标志 (4字节)
    const char startFlag[] = {'I', 'M', 'G', 'P'};  // "IMGP" 作为图像包标识
    packet.append(startFlag, 4);

    // 2. 添加图像描述信息长度 (4字节，使用网络字节序)
    QByteArray infoBytes = imageInfo.toUtf8();
    quint32 infoLength = infoBytes.size();
    infoLength = qToBigEndian(infoLength);
    packet.append(reinterpret_cast<const char*>(&infoLength), 4);
    // 3. 添加图像数据长度 (4字节，使用网络字节序)
    quint32 imageLength = imageData.size();
    imageLength = qToBigEndian(imageLength);
    packet.append(reinterpret_cast<const char*>(&imageLength), 4);
    // 4. 添加图像描述信息 (变长)
    packet.append(infoBytes);
    // 5. 添加图像数据 (变长)
    packet.append(imageData);

    return packet;
}

void ConnectionWorker::setStatus(TaskStatus status)
{
    // 只有状态真正变化时才更新
    if (m_status != status) {
        m_status = status;
        // 记录状态变化
        switch (status) {
        case TaskStatus::IDLE:
            logMessage(QString::fromLocal8Bit("状态: 空闲"));
            break;
        case TaskStatus::CONNECTING:
            logMessage(QString::fromLocal8Bit("状态: 正在连接"));
            break;
        case TaskStatus::BUSY:
            logMessage(QString::fromLocal8Bit("状态: 忙碌"));
            break;
        case TaskStatus::ERROR:
            logMessage(QString::fromLocal8Bit("状态: 错误"), true);
            break;
        case TaskStatus::UNKNOWN:
            logMessage(QString::fromLocal8Bit("状态: 未知"));
            break;
        }
    }
}
void ConnectionWorker::logMessage(const QString &message, bool isError)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString threadId = QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()));
    QString logMsg = QString("[%1] [Thread-%2] [%3:%4] %5")
                     .arg(timestamp)
                     .arg(threadId)
                     .arg(m_host)
                     .arg(m_port)
                     .arg(message);

    if (isError) {
        qWarning() << logMsg;
    } else {
        qDebug() << logMsg;
    }
}
