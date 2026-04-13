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
#include <QQueue>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <widgets/dialog.h>
#include <form.h>
#include <widgets/detect_display.h>
#include <widgets/mouldname_dialog.h>
#include <QTcpSocket>
#include <QImageWriter>
#include <QThreadPool>
#include <tcp/sendimage_task.h>


typedef QPair<QImage, QString> ImagePair; // 定义图像对类型
namespace Ui {
    class collect;
}

//连接状态
struct ConnectState{
    bool Camera=0;      //显微摄像连接状态
    bool Database=0;    //数据库连接状态
    bool stm32=0;       //单片机连接状态
    bool is_defecting=0;//是否正在进行缺陷检测
    bool tcp_connect=0; //远程服务器网络连接状态
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
    void return_init();           // 回零点设置
private slots:
    void onImageCaptured(int id, const QImage &preview);//处理相机捕获事件,自动采集和缺陷检测时的图片传输处理
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

    void on_action_collect_triggered();

    void display_dialog(QString text);//显示非模态对话框

    void close_dialog();//关闭对话框
    void on_action_defect_triggered();

    // 处理图像发送成功
    void handleImageSendSuccess(const QImage &image, const QString &imageInfo);
    // 处理图像发送失败
    void handleImageSendFailed(const QImage &image, const QString &imageInfo);

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::collect *ui;
    QCamera *Camera;
    mouldName_dialog *message_ui;
    detect_display *detect_ui;
    QQueue<ImagePair> imageQueue;//图像处理队列
    bool isProcessingQueue = false;

    void getCameras();//扫描新设备
    void startCamera();//开启摄像头采集
    void setCameraResolution(const QSize &resolution);//设置图像分辨率
    void setstatus();//设置状态栏信息
    void connectStm32();
    void closeStm32();
    QString generateFilePath();
    void processNextInQueue(); //处理保存图片任务队列
    void processNextInQueue_defect(); //处理缺陷检测传输队列
    void handleSaveResult(bool success, const QString &filePath);
    void signal_slot();//信号与槽的连接
    bool check_init();//检查零点位置是否满足要求
    bool send_image(const QImage &image,const QString &image_name);//往服务器端发送图片，并返回<发送是否成功,检测结果>
    void send_imagebyhttp(const QImage &image, const QString &image_name);//通过http发送图片
    QList<QCameraInfo> *Cameralist;

    QTimer *deviceCheckTimer;     // 用于定期检查设备的定时器

    QTimer *saveTimer;     // 自动保存
    int lastCameraCount;          // 记录上次检测到的摄像头数量
    QCameraImageCapture *imageCapture; // 用于拍照的对象
    /*状态栏控件*/
    QPushButton *statusLabel1;
    QPushButton *statusLabel2;
    QPushButton *statusLabel3;
    QPushButton *axle_position;
    QPushButton *updown_position;
    QPushButton *remote_tcp;
    SQLDatabase *my_database;            //自定义数据库
    QSqlTableModel *model;              //数据库模型
    param_manager *ui_param;            //参数管理界面
    Dialog *customDialog;               //自定义对话框，提示信息

    label *ui_label;//标注界面
    int count;//管理锁定
    QString fileSavepath;
    SerialCommunication *Serial;//串口管理

    ConnectState state;//状态,包括设备连接状态以及任务状态
    DbChecker *checker;
    /*stm32端参数*/
    quint32 axle_pulse_count;//轴向电机脉冲数量
    quint32 rotate_pulse_count;//轴向电机脉冲数量
    quint32 updown_pulse_count;//轴向电机脉冲数量
    bool limit_flag = false;//右限位标志
    bool check_dis_flag  = false;//检查是否在是否到达指定距离
    bool is_autocollect_flag = false;//是否正在采集
    /*通讯变量*/
    QByteArray receivedData_buff;
    bool waitingForStartCode = true;
    bool receivingData = false;
    bool waitingForStopCode = false;

signals:
    void finish_receive();
    void check_dis(bool flag);
    void finish_collect();
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
