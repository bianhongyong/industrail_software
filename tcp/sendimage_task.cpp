#include "sendimage_task.h"
#include <QTcpSocket>
#include <QBuffer>
#include <QImageWriter>
#include <QtEndian>
#include <QDebug>

ImageSendRunTask::ImageSendRunTask(const QImage &image, const QString &imageInfo, QObject *parent)
    : QObject(parent), m_image(image), m_imageInfo(imageInfo)
{
    // 设置任务完成后自动删除
    setAutoDelete(true);
}

ImageSendRunTask::~ImageSendRunTask()
{
    // 清理资源（如果有需要的话）
}

void ImageSendRunTask::run()
{
    // 连接服务器
    QTcpSocket socket;
    socket.connectToHost("211.81.50.204", 8080);
    if (!socket.waitForConnected(500)) {
        qDebug() << QString::fromLocal8Bit("错误: 连接服务器失败，无法发送图片:") << m_imageInfo;
        emit sendFailed(m_image, m_imageInfo);
        return;
    }
    // 将图像转换为高质量无损字节数组
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);

    // 使用最高质量的无损格式
    QImageWriter writer(&buffer, "PNG");
    writer.setQuality(100);
    writer.setCompression(0);

    if (!writer.write(m_image)){
        qDebug() << QString::fromLocal8Bit("错误: 图像写入缓冲区失败:") << writer.errorString() << "，图片:" << m_imageInfo;
        emit sendFailed(m_image, m_imageInfo);
        return;
    }

    buffer.close();

    // 创建标准化的数据包
    QByteArray data;
    // 1. 添加起始标志 (4字节)
    const char startFlag[] = {'I', 'M', 'G', 'P'};
    data.append(startFlag, 4);

    // 2. 添加图像描述信息长度 (4字节，使用网络字节序)
    QByteArray infoBytes = m_imageInfo.toUtf8();
    quint32 infoLength = infoBytes.size();
    infoLength = qToBigEndian(infoLength);
    data.append(reinterpret_cast<const char*>(&infoLength), 4);

    // 3. 添加图像数据长度 (4字节，使用网络字节序)
    quint32 imageLength = imageData.size();
    qDebug()<<imageLength;
    imageLength = qToBigEndian(imageLength);
    data.append(reinterpret_cast<const char*>(&imageLength), 4);

    // 4. 添加图像描述信息 (变长)
    data.append(infoBytes);

    // 5. 添加图像数据 (变长)
    data.append(imageData);

    // 发送数据
    qint64 bytesWritten = socket.write(data);
    if (bytesWritten == -1) {
        qDebug() << QString::fromLocal8Bit("错误: 发送数据失败，套接字错误:") << socket.errorString() << "，图片:" << m_imageInfo;
        emit sendFailed(m_image, m_imageInfo);
        return;
    }

    socket.waitForBytesWritten();

    if (!socket.waitForReadyRead()) {
        qDebug() << QString::fromLocal8Bit("错误: 等待服务器响应超时，图片:") << m_imageInfo<<socket.errorString();
        emit sendFailed(m_image, m_imageInfo);
        return;
    }

    QByteArray ack = socket.readAll();
    if (ack == "ACK") {
        emit sendSuccess(m_image, m_imageInfo);
    } 
    else {
        qDebug() << QString::fromLocal8Bit("错误: 服务器未确认接收，响应内容:") << ack << "，图片:" << m_imageInfo;
        emit sendFailed(m_image, m_imageInfo);
    }
//    socket.close();
//    qDebug()<<QString::fromLocal8Bit("关闭套接字")  ;
}
