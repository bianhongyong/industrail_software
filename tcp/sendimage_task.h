#ifndef SENDIMAGE_TASK_H
#define SENDIMAGE_TASK_H

#include <QObject>
#include <QRunnable>
#include <QImage>
class ImageSendRunTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit ImageSendRunTask(const QImage &image, const QString &imageInfo, QObject *parent = nullptr);
    ~ImageSendRunTask() override;

    // QRunnable接口实现
    void run() override;

signals:
    void sendSuccess(const QImage &image, const QString &imageInfo);
    void sendFailed(const QImage &image, const QString &imageInfo);

private:
    QImage m_image;
    QString m_imageInfo;
};



#endif // SENDIMAGE_TASK_H
