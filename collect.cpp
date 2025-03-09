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
    position(new QPushButton(this)),
    my_database(new SQLDatabase("QODBC")),
    ui_param(nullptr),
    ui_label(new label),
    count(0),
    Serial(new SerialCommunication(this)),
    pulse_count(0)
{
    ui->setupUi(this);
    /*准备工作*/
    getCameras();//扫描相机
    connectDatabase();//连接数据库与sql视图

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
    ui->move_left->setEnabled(false);
    ui->move_right->setEnabled(false);

    connectStm32();//尝试单片机的连接
    return_init();//启动应用程序时回零点
    /*将初始速度写入单片机*/
    emit ui->speed_axle->valueChanged(ui->speed_axle->value());
    emit ui->speed_rotate->valueChanged(ui->speed_axle->value());
    emit ui->speed_updown->valueChanged(ui->speed_axle->value());
}

collect::~collect()
{
    delete ui;
    delete Cameralist;
    delete ui_param;
    delete ui_label;
    delete my_database;
    if(deviceCheckTimer->isActive()) {
        deviceCheckTimer->stop();
    }
}

void collect::connectDatabase()
{
    QVariantMap params;
    params["database"] = "mould";
    params["host"] = "211.81.50.204";
    params["user"] = "bhy";
    params["password"] = "bhybhy123456";
    my_database->open(params);//打开数据库
    if(my_database->isOpen()){
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

    if(this->my_database->isOpen()){
        state.Database = 1;
        statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态: 连接"));
    }
    else{
        state.Database = 0;
        statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态: 断开"));
    }
    if(!Serial->availablePorts().contains(QString("USB-SERIAL CH340"))){
        statusLabel3->setText(QString::fromLocal8Bit("STM32单片机连接状态: 未连接"));
        state.stm32 = 0;
        ui->move_left->setEnabled(false);
        ui->move_right->setEnabled(false);
        ui->move_up->setEnabled(false);
        ui->move_down->setEnabled(false);
        ui->move_up_2->setEnabled(false);
        ui->move_up_3->setEnabled(false);
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

void collect::return_init()//让轴向电机回到右限位
{
    /*首先检查电机目前是否是在右限位位置*/
    QByteArray read_right_signal;
    read_right_signal.append(0x0D);
    bool success = Serial->sendToSTM32(read_right_signal);
    if(success){
        bool finsh =waitForSignal(this,&collect::finish_receive,500);//等待接收到信号
        if(finsh){//成功接收到
            if(right_flag){//未在右限位位置
                QByteArray data("\x0C", 1);
                // 发送数据
                success = Serial->sendToSTM32(data);
                if(success){
                    QMessageBox::information(this,QString::fromLocal8Bit("注意"),QString::fromLocal8Bit("电机正在回到初始位置，请勿操作界面"));
                }
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

    // 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 将当前日期和时间转化为纯数字的字符串格式
    QString dateTimeString = currentDateTime.toString("yyyyMMddhhmmss");
    // 生成随机文件名
    QString filePath = ui_label->config.getImagesPath()+QDir::separator()+QString("not_labeled")+QDir::separator()+ui->mould->currentText().split(QString(" "))[0]+QString::fromLocal8Bit("_")+dateTimeString+QString::fromLocal8Bit(".png");

    if (scaledImage.save(filePath)) {
        ui_label->load_file_list(false);
        QMessageBox::warning(this, QString::fromLocal8Bit("拍照成功"), QString::fromLocal8Bit("图片保存成功"));
    }
    else {
        QMessageBox::warning(this, QString::fromLocal8Bit("拍照失败"), QString::fromLocal8Bit("图片保存失败"));
    }


}

void collect::onReceiveData(QByteArray data)
{
    for (int i = 0; i < data.size(); ++i)
    {
        quint8 byte = data.at(i);

        if (waitingForStartCode)
        {
            if (byte == 0xAA) // 检测到起始码
            {
                waitingForStartCode = false;
                receivingData = true;
                receivedData_buff.clear();
            }
            else if(byte == 0xAB&&data.size()==2){
                right_flag = data.at(1);
                emit finish_receive();//通知return_init函数成功接收到信号

            }
        }
        else if (receivingData)
        {
            receivedData_buff.append(byte);
            if (receivedData_buff.size() == 4) // 已接收完32位数据
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
                quint32 axle_pulse_count = 0;
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[0]) << 24);
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[1]) << 16);
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[2]) << 8);
                axle_pulse_count |= (static_cast<quint8>(receivedData_buff[3]));
                pulse_count = axle_pulse_count;
                position->setText(QString::fromLocal8Bit("位置:")+QVariant(pulse_count).toString());
            }
            else{//异常
                qDebug() << QString::fromLocal8Bit("异常");
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
    statusLabel2->setText(QString::fromLocal8Bit("STM32单片机连接状态: 断开"));
    position->setText(QString::fromLocal8Bit("位置:")+QVariant(pulse_count).toString());
    // 创建占位控件（spacer）
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // 设置状态栏信息
    statusBar()->addWidget(statusLabel1);
    statusBar()->addWidget(statusLabel2);
    statusBar()->addWidget(statusLabel3);
    statusBar()->addWidget(position);
    statusBar()->addWidget(spacer); // 添加占位控件
    statusBar()->addWidget(position); // position 将被推到右边
    /*显示位置的地方添加回零点状态*/
    connect(position, &QPushButton::clicked, [&]() {
        QMenu menu;
        menu.addAction(QString::fromLocal8Bit("回零点"), [&]() {
            this->return_init();
        });
        // 设置菜单的宽度与按钮的宽度一致
        menu.setFixedWidth(position->width());
        QPoint buttonGlobalPos = position->mapToGlobal(QPoint(0, 0));
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

}

void collect::connectStm32()
{
    while(!Serial->availablePorts().contains(QString("USB-SERIAL CH340"))){
         QMessageBox::warning(this, QString::fromLocal8Bit("提醒"), QString::fromLocal8Bit("请检查与单片机的接线"));
    }
    state.stm32 = 1;
    if(!Serial->isOpen()){
        QMap<QString,QString> port = Serial->availablePorts();
        QString port_name = port["USB-SERIAL CH340"];
        bool isopen = Serial->openSerialPort(port_name,9600);
        if(!isopen){
            QMessageBox::warning(this, QString::fromLocal8Bit("未知错误"), QString::fromLocal8Bit("串口打开失败,请联系技术人员"));
            return;
        }
        ui->move_left->setEnabled(true);
        ui->move_right->setEnabled(true);
        ui->move_up->setEnabled(true);
        ui->move_down->setEnabled(true);
        ui->move_up_2->setEnabled(true);
        ui->move_up_3->setEnabled(true);
    }
    statusLabel3->setText(QString::fromLocal8Bit("STM32单片机连接状态: 连接"));
}

void collect::closeStm32()
{
    state.stm32 = 0;
    ui->move_left->setEnabled(false);
    ui->move_right->setEnabled(false);
    ui->move_up->setEnabled(false);
    ui->move_down->setEnabled(false);
    ui->move_up_2->setEnabled(false);
    ui->move_up_3->setEnabled(false);
    statusLabel3->setText(QString::fromLocal8Bit("STM32单片机连接状态: 未连接"));
    Serial->closeSerialPort();
}

void collect::on_param_clicked()
{
    if(my_database->isOpen()){
        ui_param->show();
    }
    else{
         QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("数据库未连接"));
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
