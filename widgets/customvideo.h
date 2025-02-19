#ifndef CUSTOMVIDEO_H
#define CUSTOMVIDEO_H

#include <QVideoWidget>
#include <QObject>
#include <QList>
#include <QCameraInfo>
#include <QCamera>
class CustomVideo : public QVideoWidget
{
    Q_OBJECT

public:

    CustomVideo(QWidget *parent = nullptr);
    ~CustomVideo();

private:

};

#endif // CUSTOMVIDEO_H
