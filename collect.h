#ifndef COLLECT_H
#define COLLECT_H
#include <QCamera>
#include <QMainWindow>
#include <QVideoWidget>
#include <QCameraInfo>
#include <QTimer>
#include <QCameraImageCapture>
namespace Ui {
class collect;
}

class collect : public QMainWindow
{
    Q_OBJECT

public:
    explicit collect(QWidget *parent = nullptr);
    ~collect();

protected slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void checkCameraDevices();    // 检查摄像头设备变化
    void handleDeviceChange();    // 处理设备变化
private slots:
    void onImageCaptured(int id, const QImage &preview);
    void on_action_shot_triggered();
    
private:
    Ui::collect *ui;
    QCamera *Camera;
    void getCameras();//扫描新设备
    void startCamera();//开启摄像头采集
    void setCameraResolution(const QSize &resolution);
    QList<QCameraInfo> *Cameralist;

    QTimer *deviceCheckTimer;     // 用于定期检查设备的定时器
    int lastCameraCount;          // 记录上次检测到的摄像头数量
    QCameraImageCapture *imageCapture; // 用于拍照的对象
};

#endif // COLLECT_H
