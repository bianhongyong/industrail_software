#ifndef COLLECT_H
#define COLLECT_H
#include <QCamera>
#include <QMainWindow>
#include <QVideoWidget>
#include <QCameraInfo>
#include <QTimer>
#include <QCameraImageCapture>
#include <SqlDatabaseManager.h>
#include <QLabel>
#include <widgets/CustomTabView.h>
#include <param_manager.h>
#include <label.h>
#include <global.h>
#include <ConfigManager.h>
#include <Qdir>
namespace Ui {
class collect;
}

class collect : public QMainWindow
{
    Q_OBJECT

public:
    explicit collect(QWidget *parent = nullptr);
    ~collect();
    void connectDatabase();
    void populateMouldComboBox();
protected slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void checkCameraDevices();    // 检查摄像头设备变化
    void handleDeviceChange();    // 处理设备变化
private slots:
    void onImageCaptured(int id, const QImage &preview);
    void on_action_shot_triggered();
    void on_param_clicked();

    void on_lock_clicked();

    void on_action_label_triggered();

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
    QLabel *statusLabel1;
    QLabel *statusLabel2;
    SQLDatabase *my_database;            //自定义数据库
    QSqlTableModel *model;              //数据库模型
    param_manager *ui_param;

    label *ui_label;//标注界面
    int count;//管理锁定
    QString fileSavepath;

};

#endif // COLLECT_H
