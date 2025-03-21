#include "collect.h"
#include "./ui_collect.h"
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>
#include <QUuid>
#include <QLabel>
#include "widgets/CustomTabView.h"
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
collect::collect(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::collect),
    Camera(nullptr),
    Cameralist(new QList<QCameraInfo>),
    deviceCheckTimer(new QTimer(this)),
    lastCameraCount(0),
    imageCapture(nullptr),
    statusLabel1(new QPushButton(this)),  // 初始化第一个 QLabel
    statusLabel2(new QPushButton(this)),   // 初始化第二个 QLabel
    statusLabel3(new QPushButton(this)),
    axle_position(new QPushButton(this)),
    updown_position(new QPushButton(this)),
    my_database(new SQLDatabase("QODBC")),
    ui_param(nullptr),
    customDialog(nullptr),
    ui_label(new label),
    count(0),
    Serial(new SerialCommunication(this)),
    checker(new DbChecker()),
    axle_pulse_count(0),
    rotate_pulse_count(0),
    updown_pulse_count(0)
{
    ui->setupUi(this);
    /*准备工作*/
    getCameras();//扫描相机

    ui->size->setCurrentIndex(3);
    lastCameraCount = Cameralist->count();


    // 设置定时器
    deviceCheckTimer->setInterval(500);  // 每秒检查一次
    connect(deviceCheckTimer, &QTimer::timeout, this, &collect::checkCameraDevices);
    deviceCheckTimer->start();
    imageCapture = new QCameraImageCapture(Camera, this);
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &collect::onImageCaptured);

    //设置状态栏
    setstatus();
    connect(ui_param,&param_manager::database_flush,this,[=](){
        populateMouldComboBox();
    });//刷新Qcombox内容
    connect(ui_label,&label::close,this,[=](){
        ui_label->hide();
        this->show();
    });//实现界面的来回切换
    connect(Serial,&SerialCommunication::serial_portFailopen,this,[=](){
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("串口打开失败，请检查电脑与单片机的接线是否接好"));
    });//实现界面的来回切换
    connect(Serial,&SerialCommunication::serial_senddata,this,&collect::onReceiveData);
    connect(this,&collect::check_dis,this,[=](bool flag){
    });
    connect(this,&collect::finish_collect,[=]{
        close_dialog();
    });
    ui->move_left->setEnabled(false);
    ui->move_right->setEnabled(false);
    connectDatabase();//连接数据库与sql视图
    connectStm32();//尝试单片机的连接
    return_init();//启动应用程序时回零点
    /*将初始速度写入单片机*/
    if(state.stm32==1){
        emit ui->speed_axle->valueChanged(ui->speed_axle->value());
        emit ui->speed_rotate->valueChanged(ui->speed_axle->value());
        emit ui->speed_updown->valueChanged(ui->speed_axle->value());
    }


}

collect::~collect()
{
    delete ui;
    delete Cameralist;
    delete ui_param;
    delete ui_label;
    delete my_database;
    checker->requestInterruption();
    checker->wait();
    delete checker;
    if(deviceCheckTimer->isActive()) {
        deviceCheckTimer->stop();
    }
}

void collect::connectDatabase()
{

    if(my_database->isOpen()){
        return;
    }

    QVariantMap params;
    params["database"] = "mould";
    params["host"] = "211.81.50.204";
    params["user"] = "bhy";
    params["password"] = "bhybhy123456";

    if(my_database->open(params)){
      state.Database=1;
      // 创建数据库模型
      model = new QSqlTableModel(nullptr, my_database->database());
      model->setTable("mould"); // 设置要操作的表名
      model->setEditStrategy(QSqlTableModel::OnManualSubmit); // 设置编辑策略为手动提交
      model->select();
      if(ui_param!=nullptr){
        delete ui_param;
        ui_param = new param_manager(model);
      }
      else{
        ui_param = new param_manager(model);
      }
      //ComboBox信息更新
      populateMouldComboBox();
      statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态:连接"));


//      connect(checker,&DbChecker::pingResult,this,[=](bool isActive){
//        if(isActive){
//            state.Database=1;
//            statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态:连接"));
//        }
//        else{
//            state.Database=0;
//            statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态:未连接"));
//            checker->requestInterruption();
//            checker->wait();
//            my_database->close();
//        }
//      });

//      checker->start();
    }
}

void collect::getCameras()
{
    ui->comboBox->clear();
    *Cameralist=QCameraInfo::availableCameras();
    QStringList description_list;
    for(int i = 0;i<Cameralist->count();++i) {
        description_list.append(Cameralist->at(i).description());
    }
    ui->comboBox->addItems(description_list);
    if(description_list.contains(QString("UVC Camera"))){
        int id = description_list.indexOf("UVC Camera");
        ui->comboBox->setCurrentIndex(id);
    }
}
void collect::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if(Camera!=nullptr){
        Camera->stop();
        delete Camera;
        Camera = nullptr;
    }

    if(ui->comboBox->currentIndex()!=-1){//防止comboBox被清空的情况
        Camera = new QCamera(Cameralist->at(ui->comboBox->currentIndex()), this);
        Camera->setViewfinder(ui->video);
        Camera->start();
    }

}
void collect::checkCameraDevices()
{
    QList<QCameraInfo> currentCameras = QCameraInfo::availableCameras();
    if(currentCameras.count() != lastCameraCount) {
        handleDeviceChange();
    }
    QStringList description_list;
    for(int i = 0;i<currentCameras.count();++i) {
        description_list.append(currentCameras.at(i).description());
    }


    if(description_list.contains("UVC Camera")){
        state.Camera = 1;
        statusLabel1->setText(QString::fromLocal8Bit("显微摄像头连接状态: 连接"));
    }
    else{
        state.Camera = 0;
        statusLabel1->setText(QString::fromLocal8Bit("显微摄像头连接状态: 断开"));
    }

    if(!Serial->availablePorts().contains(QString("USB-SERIAL CH340"))){
        statusLabel3->setText(QString::fromLocal8Bit("STM32单片机连接状态: 未连接"));
        closeStm32();
    }

}

void collect::handleDeviceChange()
{
    QList<QCameraInfo> currentCameras = QCameraInfo::availableCameras();
    int currentCount = currentCameras.count();

    if(currentCount > lastCameraCount) {
        // 有新设备插入
        QMessageBox::information(this, QString::fromLocal8Bit("设备通知"),
                               QString::fromLocal8Bit("检测到新的摄像头设备连接"));
    } else {
        // 有设备断开
        QMessageBox::warning(this, QString::fromLocal8Bit("设备通知"),
                           QString::fromLocal8Bit("检测到摄像头设备断开"));

        // 如果当前使用的摄像头被断开，需要停止摄像头
        if(Camera && !currentCameras.contains(Cameralist->at(ui->comboBox->currentIndex()))) {
            Camera->stop();
            delete Camera;
            Camera = nullptr;
        }
    }

    // 更新设备列表
    *Cameralist = currentCameras;
    getCameras();
    lastCameraCount = currentCount;
}

void collect::return_init()//让轴向电机回到限位位置，上下电机和轴向电机都得回
{
    if(state.stm32==0){
        QMessageBox::information(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit("单片机未连接,无法回零点"));
        return;
    }
    if(state.Database==0){
        QMessageBox::information(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit("数据库未连接,无法回零点"));
        return;
    }
    /*首先检查电机目前是否限位位置*/
    QByteArray read_right_signal;
    read_right_signal.append(0x0D);
    bool success = Serial->sendToSTM32(read_right_signal);
    if(success){
        bool finsh =waitForSignal(this,&collect::finish_receive,5000);//等待接收到信号
        if(finsh){//成功接收到
            QByteArray data("\x0C", 1);
            // 发送数据
            success = Serial->sendToSTM32(data);
            if(success){
                QMessageBox::information(this,QString::fromLocal8Bit("注意"),QString::fromLocal8Bit("电机正在回到初始位置，请勿操作界面"));
            }
        }
        else{
            QMessageBox::information(this,QString::fromLocal8Bit("未知错误"),QString::fromLocal8Bit("读限位标志失败，请联系技术支持"));
        }
    }
    else{
        QMessageBox::information(this,QString::fromLocal8Bit("未知错误"),QString::fromLocal8Bit("数据发送失败，请联系技术支持"));
    }
}

void collect::onImageCaptured(int id, const QImage &preview)
{
    Q_UNUSED(id);
    QString resolution = ui->size->currentText();
    QStringList res = resolution.split(QString::fromLocal8Bit("×"));//TODO 编码隐患,导致分辨率长宽分离不出来
    int width = res[0].toInt();

    int height = res[1].toInt();
    QImage scaledImage = preview.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageQueue.enqueue(scaledImage);


    // 如果没有处理线程在运行，则启动一个
    if (!isProcessingQueue){
        processNextInQueue();
    }
}

void collect::onReceiveData(QByteArray data)
{

    /*通讯规定
    0xAA 接收轴向电机脉冲起始标志
    0x55 接收轴向电机脉冲结束标志

    0xAB 接收是否在限位标志
    0xAC 开始接收上下距离和水平距离是否调整到指定位置标志
    0xAD 接收拍照标志
    0XAE 接收自动采集完成标志
    0xB0 接收旋转电机脉冲起始标志
    0xB1 接收旋转电机脉冲结束标志
    0xB2 接收旋转电机脉冲起始标志
    0xB3 接收旋转电机脉冲结束标志
    0xB4 自动采集完成标志
    */

    int data_size=4;
    for (int i = 0; i < data.size(); ++i)
    {
        quint8 byte = data.at(i);

        if (waitingForStartCode)
        {
            if (byte == 0xAA) // 轴向电机脉冲起始标志
            {
                waitingForStartCode = false;
                receivingData = true;
                receivedData_buff.clear();
                data_size=4;
            }
            else if(byte==0xB0){// 旋转电机脉冲起始标志
                waitingForStartCode = false;
                receivingData = true;
                receivedData_buff.clear();
                data_size=4;
            }
            else if(byte==0xB2){// 上下电机脉冲起始标志
                waitingForStartCode = false;
                receivingData = true;
                receivedData_buff.clear();
                data_size=4;
            }
            else if(byte==0xB4){
                emit finish_collect();
            }
            else if(byte == 0xAB&&data.size()==2){
                limit_flag = data.at(1);
                emit finish_receive();//通知return_init函数成功接收到信号
            }
            else if(byte == 0xAC&&data.size()==2){
                check_dis_flag = QVariant(data.at(1)).toBool();
                emit check_dis(QVariant(data.at(1)).toBool());//检查距离是否合法
            }
            else if(byte==0xAE&&data.size()==1){
                emit ui->action_shot->triggered();  //触发拍照函数
            }
        }
        else if (receivingData)
        {
            receivedData_buff.append(byte);
            if (receivedData_buff.size() == data_size) // 已接收完32位数据
            {
                receivingData = false;
                waitingForStopCode = true;
            }
        }
        else if (waitingForStopCode)
        {
            if (byte == 0x55) // 检测到终止码
            {
                waitingForStopCode = false;
                waitingForStartCode = true;
                // 解析接收到的32位数据
                axle_pulse_count=0;
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[0]) << 24);
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[1]) << 16);
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[2]) << 8);
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[3]));
                axle_position->setText(QString::fromLocal8Bit("轴向位置:")+QVariant(axle_pulse_count).toString());
            }
            else if(byte == 0xB1){
                waitingForStopCode = false;
                waitingForStartCode = true;
                // 解析接收到的32位数据
                rotate_pulse_count=0;
                rotate_pulse_count |= (static_cast<quint8>(receivedData_buff[0]) << 24);
                rotate_pulse_count |= (static_cast<quint8>(receivedData_buff[1]) << 16);
                rotate_pulse_count |= (static_cast<quint8>(receivedData_buff[2]) << 8);
                rotate_pulse_count |= (static_cast<quint8>(receivedData_buff[3]));
                axle_position->setText(QString::fromLocal8Bit("旋转位置:")+QVariant(rotate_pulse_count).toString());
            }
            else if(byte == 0xB3){
                waitingForStopCode = false;
                waitingForStartCode = true;
                // 解析接收到的32位数据
                updown_pulse_count=0;
                updown_pulse_count |= (static_cast<quint8>(receivedData_buff[0]) << 24);
                updown_pulse_count |= (static_cast<quint8>(receivedData_buff[1]) << 16);
                updown_pulse_count |= (static_cast<quint8>(receivedData_buff[2]) << 8);
                updown_pulse_count |= (static_cast<quint8>(receivedData_buff[3]));
                updown_position->setText(QString::fromLocal8Bit("上下位置:")+QVariant(updown_pulse_count).toString());


            }
            else{//异常
                //qDebug() << QString::fromLocal8Bit("异常");
                waitingForStopCode = false;
                waitingForStartCode = true;
                break;
            }
        }
    }
}


void collect::on_action_shot_triggered()
{
    if (Camera && Camera->state() == QCamera::ActiveState) {
        if(!imageCapture->isReadyForCapture()){
            delete imageCapture;
            imageCapture = new QCameraImageCapture(Camera, this);
            connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &collect::onImageCaptured);
        }
        imageCapture->capture();
    }
    else {
        QMessageBox::warning(this, QString::fromLocal8Bit("拍照失败"), QString::fromLocal8Bit("摄像头未启动"));
    }
}
void collect::setCameraResolution(const QSize &resolution)
{
    if (Camera) {
        QCameraViewfinderSettings viewfinderSettings;
        viewfinderSettings.setResolution(resolution);
        Camera->setViewfinderSettings(viewfinderSettings);
    }
}

void collect::setstatus()
{
    statusLabel1->setText(QString::fromLocal8Bit("显微摄像头连接状态: 断开"));
    statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态: 断开"));
    statusLabel3->setText(QString::fromLocal8Bit("STM32单片机连接状态: 断开"));
    axle_position->setText(QString::fromLocal8Bit("轴向位置:")+QVariant(axle_pulse_count).toString());
    updown_position->setText(QString::fromLocal8Bit("上下位置:")+QVariant(updown_pulse_count).toString());
    // 创建占位控件（spacer）
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // 设置状态栏信息
    statusBar()->addWidget(statusLabel1);
    statusBar()->addWidget(statusLabel2);
    statusBar()->addWidget(statusLabel3);
    //statusBar()->addWidget(position);
    statusBar()->addWidget(spacer); // 添加占位控件
    statusBar()->addWidget(axle_position); // position 将被推到右边
    statusBar()->addWidget(updown_position); // position 将被推到右边
    /*显示位置的地方添加回零点状态*/
    connect(axle_position, &QPushButton::clicked, [&]() {
        QMenu menu;
        menu.addAction(QString::fromLocal8Bit("回零点"), [&]() {
            this->return_init();
        });
        // 设置菜单的宽度与按钮的宽度一致
        menu.setFixedWidth(axle_position->width());
        QPoint buttonGlobalPos = axle_position->mapToGlobal(QPoint(0, 0));
        // 计算菜单的位置，使其显示在按钮的正上方
        QPoint menuPos = buttonGlobalPos - QPoint(0, menu.sizeHint().height());
        // 在按钮上方弹出菜单
        menu.exec(menuPos);
    });

    /*串口*/
    connect(statusLabel3, &QPushButton::clicked, [&]() {
        QMenu menu;
        menu.addAction(QString::fromLocal8Bit("断开单片机连接"), [&](){
            closeStm32();
        });
        menu.addAction(QString::fromLocal8Bit("重新连接"), [&](){
            connectStm32();
        });
        // 设置菜单的宽度与按钮的宽度一致
        menu.setFixedWidth(statusLabel3->width());
        QPoint buttonGlobalPos = statusLabel3->mapToGlobal(QPoint(0, 0));
        // 计算菜单的位置，使其显示在按钮的正上方
        QPoint menuPos = buttonGlobalPos - QPoint(0, menu.sizeHint().height());
        // 在按钮上方弹出菜单
        menu.exec(menuPos);
    });

    /*数据库*/
    connect(statusLabel2, &QPushButton::clicked, [&]() {
        QMenu menu;
        menu.addAction(QString::fromLocal8Bit("重新连接"), [&](){
            if(state.Database==0)connectDatabase();
        });
        // 设置菜单的宽度与按钮的宽度一致
        menu.setFixedWidth(statusLabel2->width());
        QPoint buttonGlobalPos = statusLabel2->mapToGlobal(QPoint(0, 0));
        // 计算菜单的位置，使其显示在按钮的正上方
        QPoint menuPos = buttonGlobalPos - QPoint(0, menu.sizeHint().height());
        // 在按钮上方弹出菜单
        menu.exec(menuPos);
    });
}

void collect::connectStm32()
{
    if(!Serial->isOpen()){
        QMap<QString,QString> port = Serial->availablePorts();
        QString port_name = port["USB-SERIAL CH340"];
        bool isopen = Serial->openSerialPort(port_name,9600);
        if(!isopen){
            return;
        }
        ui->speed_axle->setEnabled(true);
        ui->speed_rotate->setEnabled(true);
        ui->speed_updown->setEnabled(true);
        ui->move_left->setEnabled(true);
        ui->move_right->setEnabled(true);
        ui->move_up->setEnabled(true);
        ui->move_down->setEnabled(true);
        ui->move_up_2->setEnabled(true);
        ui->move_up_3->setEnabled(true);
    }
    state.stm32 = 1;
    statusLabel3->setText(QString::fromLocal8Bit("STM32单片机连接状态: 连接"));
}

void collect::closeStm32()
{
    state.stm32 = 0;
    ui->speed_axle->setEnabled(false);
    ui->speed_rotate->setEnabled(false);
    ui->speed_updown->setEnabled(false);
    ui->move_left->setEnabled(false);
    ui->move_right->setEnabled(false);
    ui->move_up->setEnabled(false);
    ui->move_down->setEnabled(false);
    ui->move_up_2->setEnabled(false);
    ui->move_up_3->setEnabled(false);
    statusLabel3->setText(QString::fromLocal8Bit("STM32单片机连接状态: 未连接"));
    Serial->closeSerialPort();
}

void collect::processNextInQueue()
{
    // 如果队列为空，停止处理
    if (imageQueue.isEmpty()){
        isProcessingQueue = false;
        return;
    }
    // 标记为正在处理
        isProcessingQueue = true;

        // 从队列中取出一个图像
        QImage image = imageQueue.dequeue();
        // 生成文件路径
        QString filePath = generateFilePath();

        // 异步保存图像
        QFuture<bool> future = QtConcurrent::run([=]() {
            return image.save(filePath);
        });

        // 设置完成后的处理
        QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>(this);
        connect(watcher, &QFutureWatcher<bool>::finished, this, [=]() {
            // 处理保存结果
            handleSaveResult(watcher->result(), filePath);

            // 清理资源
            watcher->deleteLater();

            // 处理队列中的下一个图像
            QTimer::singleShot(0, this, &collect::processNextInQueue);
        });

        watcher->setFuture(future);
}
QString collect::generateFilePath()
{
    // 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 将当前日期和时间转化为纯数字的字符串格式
    QString dateTimeString = currentDateTime.toString("yyyyMMddhhmmsszzz");

    // 生成文件名: 模具名_时间戳.png
    QString filePath = ui_label->config.getImagesPath() +
                       QDir::separator() +
                       QString("not_labeled") +
                       QDir::separator() +
                       ui->mould->currentText().split(QString(" "))[0] +
                       QString::fromLocal8Bit("_") +
                       dateTimeString +
                       QString::fromLocal8Bit(".png");
    return filePath;
}
void collect::handleSaveResult(bool success, const QString &filePath)
{
    if (success) {
        ui_label->load_file_list(false);
//        QMessageBox::information(this,
//                               QString::fromLocal8Bit("拍照成功"),
//                               QString::fromLocal8Bit("图片保存成功"));
    }
    else {
//        QMessageBox::warning(this,
//                           QString::fromLocal8Bit("拍照失败"),
//                           QString::fromLocal8Bit("图片保存失败"));
    }
}
void collect::on_param_clicked()
{
    if(my_database->isOpen()){
        ui_param->show();
    }
    else{
         QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("数据库未连接,无法打开"));
    }
}

void collect::populateMouldComboBox()
{
    if (!my_database->isOpen()) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("数据库未连接"));
        return;
    }

    ui->mould->clear();
    for(int i = 0;i<model->rowCount();++i){
        QString display_text = model->record(i).value(0).toString()+QString::fromLocal8Bit(" 垂直距离:")+model->record(i).value(1).toString()+QString::fromLocal8Bit(" 水平距离:")+model->record(i).value(2).toString();
        ui->mould->addItem(display_text);
    }
}


void collect::on_lock_clicked()
{
    if(count==0){
        ui->mould->setEnabled(false);
        count = 1;
    }
    else{
        ui->mould->setEnabled(true);
        count = 0;
    }
}


void collect::on_action_label_triggered()
{
    this->hide();
    ui_label->show();
}

void collect::on_move_left_pressed()
{
    // 创建包含 0x00 的 QByteArray
    QByteArray data("\x01", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_left_released()
{
    // 创建包含 0x00 的 QByteArray
    QByteArray data("\x03", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_right_pressed()
{
    // 创建包含 0x00 的 QByteArray
    QByteArray data("\x02", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_right_released()
{
    // 创建包含 0x00 的 QByteArray
    QByteArray data("\x03", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_up_pressed()
{
    // 创建包含 0x00 的 QByteArray
    QByteArray data("\x04", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_up_released()
{
    QByteArray data("\x06", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_down_pressed()
{
    // 创建包含 0x00 的 QByteArray
    QByteArray data("\x05", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_down_released()
{
    // 创建包含 0x00 的 QByteArray
    QByteArray data("\x06", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_speed_axle_valueChanged(int arg1)
{
    // 提取低 16 位
    quint16 low16 = static_cast<quint16>(arg1 & 0xFFFF);
    // 将低 16 位拆分为两个字节
    quint8 byte1 = static_cast<quint8>((low16 >> 8) & 0xFF); // 高字节
    quint8 byte2 = static_cast<quint8>(low16 & 0xFF);        // 低字节
    // 准备发送的数据（5个字节）
    QByteArray dataToSend;
    dataToSend.append(0x07);//开始接收码
    dataToSend.append('\x00');//电机类型码
    dataToSend.append(byte1);//电机速度高八位
    dataToSend.append(byte2);//电机速度低八位
    dataToSend.append(0x08);//停止接收码
    // 发送数据
    bool success = Serial->sendToSTM32(dataToSend);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_speed_updown_valueChanged(int arg1)
{
    // 提取低 16 位
    quint16 low16 = static_cast<quint16>(arg1 & 0xFFFF);
    // 将低 16 位拆分为两个字节
    quint8 byte1 = static_cast<quint8>((low16 >> 8) & 0xFF); // 高字节
    quint8 byte2 = static_cast<quint8>(low16 & 0xFF);        // 低字节
    // 准备发送的数据（5个字节）
    QByteArray dataToSend;
    dataToSend.append(0x07);//开始接收码
    dataToSend.append('\x01');//电机类型码
    dataToSend.append(byte1);//电机速度高八位
    dataToSend.append(byte2);//电机速度低八位
    dataToSend.append(0x08);//停止接收码
    // 发送数据
    bool success = Serial->sendToSTM32(dataToSend);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_speed_rotate_valueChanged(int arg1)
{
    // 提取低 16 位
    quint16 low16 = static_cast<quint16>(arg1 & 0xFFFF);
    // 将低 16 位拆分为两个字节
    quint8 byte1 = static_cast<quint8>((low16 >> 8) & 0xFF); // 高字节
    quint8 byte2 = static_cast<quint8>(low16 & 0xFF);        // 低字节
    // 准备发送的数据（5个字节）
    QByteArray dataToSend;
    dataToSend.append(0x07);//开始接收码
    dataToSend.append('\x02');//电机类型码
    dataToSend.append(byte1);//电机速度高八位
    dataToSend.append(byte2);//电机速度低八位
    dataToSend.append(0x08);//停止接收码
    // 发送数据
    bool success = Serial->sendToSTM32(dataToSend);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_up_2_pressed()//正转按下
{
    QByteArray data("\x09", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}


void collect::on_move_up_2_released()//正转释放
{
    QByteArray data("\x0B", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}

void collect::on_move_up_3_pressed()//反转按下
{
    QByteArray data("\x0A", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}

void collect::on_move_up_3_released()//反转释放
{
    QByteArray data("\x0B", 1);
    // 发送数据
    bool success = Serial->sendToSTM32(data);
    if(!success){
        QMessageBox::information(this,QString::fromLocal8Bit("故障"),QString::fromLocal8Bit("数据发送失败,请联系技术支持"));
    }
}

void collect::on_action_collect_triggered()
{
    QMessageBox msgBox;
    msgBox.setInformativeText(QString::fromLocal8Bit("确认开始自动采集？"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret==QMessageBox::Cancel){
        return;
    }

    if(state.stm32==0){
        QMessageBox::information(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit("单片机未连接，无法启动自动采集功能"));
        return;
    }
    if(state.Database==0){
        QMessageBox::information(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit("数据库未连接，无法启动自动采集功能"));
        return;
    }
    if(ui->comboBox->currentText()!=QString::fromLocal8Bit("UVC Camera")){
        QMessageBox::information(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit("未连接或未正确选择显微摄像头，无法启动自动采集功能"));
        return;
    }
    /*TODO 要检查初始状态是否达到要求
    1.将数据库里的水平和上下距离发给单片机，待单片机校准(起始终止码: 0x0E 0x0F)
    2.等待单片机的校准信号(0xAC)
    3.若收到的校准信号合格，则开始回零点.不合格则提示用户调整 信号与槽机制
    4.回零点完成后，提示用户正在采集，并显示采集信息(可商量)
    */
    QByteArray dataToSend;
    dataToSend.append(0x0E);  // 起始码
    quint16 updown = model->record(ui->mould->currentIndex()).value(1).toUInt();
    quint8 updown_high = static_cast<quint8>((updown >> 8) & 0xFF);   // 高字节
    quint8 updown_low = static_cast<quint8>(updown & 0xFF);           // 低字节
    dataToSend.append(updown_high);
    dataToSend.append(updown_low);
    quint16 horizon = model->record(ui->mould->currentIndex()).value(2).toUInt();
    quint8 horizon_high = static_cast<quint8>((horizon >> 8) & 0xFF);  // 高字节
    quint8 horizon_low = static_cast<quint8>(horizon & 0xFF);          // 低字节
    dataToSend.append(horizon_high);
    dataToSend.append(horizon_low);
    dataToSend.append(0x0F);  // 终止码
    bool success = Serial->sendToSTM32(dataToSend);
    if (!success) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("数据发送失败，请联系技术支持"));
        return;
    }
    bool calibrationSuccess = waitForSignal(this, &collect::check_dis,5000);  // 等待 0.5 秒
    if(!calibrationSuccess){//接收信号超时
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未收到校准信号，请检查设备"));
        return;
    }

    if(check_dis_flag==0){//摄像头没有调整到指定位置
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("摄像头未到达指定位置"));
        return;
    }
    else{//若调整到指定位置就回零点
        return_init();
    }
    /*
    1.发送0x10信号通知单片机要开始自动采集了
    2.将数据库里所选模具的轴向视野宽度和旋转视野宽度一并发送给单片机,这里后期可能不仅仅要发送这些信息，还有很多其他的信息(TODO)。
    3.发送0x11信号通知单片机发送完毕
    4.等待单片机发送拍照信号
    5.接收到拍照信号，调用拍照函数，发送拍照完成标志
    6.接收自动采集完成标志，提示用户采集完毕更换材料
    */
    dataToSend.clear();
    dataToSend.append(0x10);  // 起始码
    quint16 axle_view_count = model->record(ui->mould->currentIndex()).value(3).toUInt();
    quint8 axle_high = static_cast<quint8>((axle_view_count >> 8) & 0xFF);   // 高字节
    quint8 axle_low = static_cast<quint8>(axle_view_count & 0xFF);           // 低字节
    dataToSend.append(axle_high);
    dataToSend.append(axle_low);
    quint16 rotate_view_count = model->record(ui->mould->currentIndex()).value(4).toUInt();
    quint8 rotate_high = static_cast<quint8>((rotate_view_count >> 8) & 0xFF);  // 高字节
    quint8 rotate_low = static_cast<quint8>(rotate_view_count & 0xFF);          // 低字节
    dataToSend.append(rotate_high);
    dataToSend.append(rotate_low);
    dataToSend.append(0x11);  // 终止码
    success = Serial->sendToSTM32(dataToSend);
    if (!success) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("数据发送失败，请联系技术支持"));
        return;
    }
    display_dialog(QString::fromLocal8Bit("正在自动采集中请等待...."));
}

void collect::display_dialog(QString text)
{
    customDialog = new Dialog();
    customDialog->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    customDialog->setModal(true); // 设置为非模态
    customDialog->setWindowFlags(customDialog->windowFlags() & ~Qt::WindowCloseButtonHint);// 禁用关闭按钮
    QLabel *label = new QLabel(text, customDialog);
    QVBoxLayout *layout = new QVBoxLayout(customDialog);
    layout->addWidget(label);
    customDialog->setLayout(layout);
    // 显示对话框
    customDialog->show();
}

void collect::close_dialog()
{
    if (customDialog) {
        customDialog->close();
        customDialog = nullptr;
    }
}

