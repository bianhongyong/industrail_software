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
#include <serialcommunication.h>
#include <QEventLoop>
namespace Ui {
class collect;
}

//连接状态
struct ConnectState{
    int Camera=0;
    int Database=0;
    int stm32=0;
};

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
    void return_init();           //回零点设置
private slots:
    void onImageCaptured(int id, const QImage &preview);
    void onReceiveData(QByteArray data);
    void on_action_shot_triggered();//拍照槽函数
    void on_param_clicked();//参数管理

    void on_lock_clicked();//锁定参数选择下拉框为不可编辑

    void on_action_label_triggered();//切换到标注界面
    //电机控制按钮槽函数
    void on_move_left_pressed();

    void on_move_left_released();

    void on_move_right_pressed();

    void on_move_right_released();

    void on_move_up_pressed();

    void on_move_up_released();

    void on_move_down_pressed();

    void on_move_down_released();

    void on_speed_axle_valueChanged(int arg1);

    void on_speed_updown_valueChanged(int arg1);

    void on_speed_rotate_valueChanged(int arg1);

    void on_move_up_2_pressed();

    void on_move_up_2_released();

    void on_move_up_3_released();

    void on_move_up_3_pressed();

private:
    Ui::collect *ui;
    QCamera *Camera;

    void getCameras();//扫描新设备
    void startCamera();//开启摄像头采集
    void setCameraResolution(const QSize &resolution);//设置图像分辨率
    void setstatus();//设置状态栏信息
    void connectStm32();
    void closeStm32();
    QList<QCameraInfo> *Cameralist;

    QTimer *deviceCheckTimer;     // 用于定期检查设备的定时器
    int lastCameraCount;          // 记录上次检测到的摄像头数量
    QCameraImageCapture *imageCapture; // 用于拍照的对象
    /*状态栏控件*/
    QPushButton *statusLabel1;
    QPushButton *statusLabel2;
    QPushButton *statusLabel3;
    QPushButton *position;
    SQLDatabase *my_database;            //自定义数据库
    QSqlTableModel *model;              //数据库模型
    param_manager *ui_param;

    label *ui_label;//标注界面
    int count;//管理锁定
    QString fileSavepath;
    SerialCommunication *Serial;//串口管理

    ConnectState state;//连接状态
    /*stm32端参数*/
    int pulse_count;//脉冲数量
    bool right_flag = false;//右限位标志
    /*通讯变量*/
    QByteArray receivedData_buff;
    bool waitingForStartCode = true;
    bool receivingData = false;
    bool waitingForStopCode = false;
signals:
    void finish_receive();
};


// 阻塞函数，等待指定信号触发
template <typename ObjectType, typename SignalType>
bool waitForSignal(ObjectType object, SignalType signal, int timeout = -1) {
    QEventLoop loop;
    bool timedOut = false; // 标志是否超时

    // 连接信号到槽函数，信号触发时退出事件循环
    QObject::connect(object, signal, &loop, [&]() {
        loop.quit();
    });

    // 如果设置了超时时间，连接超时信号
    if (timeout >= 0) {
        QTimer::singleShot(timeout, &loop, [&]() {
            timedOut = true; // 设置超时标志
            loop.quit();
        });
    }

    // 进入事件循环，等待信号触发或超时
    loop.exec();

    // 检查是否超时
    return !timedOut; // 如果 timedOut 为 true，说明超时，返回 false
}
#endif // COLLECT_H
