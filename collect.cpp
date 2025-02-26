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
    statusLabel1(new QLabel(this)),  // 初始化第一个 QLabel
    statusLabel2(new QLabel(this)),   // 初始化第二个 QLabel
    my_database(new SQLDatabase("QODBC")),
    ui_param(nullptr),
    ui_label(new label),
    count(0)
{
    ui->setupUi(this);
    /*准备工作*/
    getCameras();//扫描相机
    connectDatabase();//连接数据库与sql视图

    ui->size->setCurrentIndex(3);
    lastCameraCount = Cameralist->count();
    //qDebug()<<lastCameraCount;
    // 设置定时器
    deviceCheckTimer->setInterval(500);  // 每秒检查一次
    connect(deviceCheckTimer, &QTimer::timeout, this, &collect::checkCameraDevices);
    deviceCheckTimer->start();
    imageCapture = new QCameraImageCapture(Camera, this);
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &collect::onImageCaptured);


    statusLabel1->setText(QString::fromLocal8Bit("显微摄像头连接状态: 断开"));
    statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态: 断开"));
    // 设置状态栏信息
    statusBar()->addWidget(statusLabel1);
    statusBar()->addWidget(statusLabel2);


    connect(ui_param,&param_manager::database_flush,this,[=](){
        populateMouldComboBox();
    });//刷新Qcombox内容
    connect(ui_label,&label::close,this,[=](){
        ui_label->hide();
        this->show();
    });//实现界面的来回切换

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
        statusLabel1->setText(QString::fromLocal8Bit("显微摄像头连接状态: 连接"));
    }
    else{
        statusLabel1->setText(QString::fromLocal8Bit("显微摄像头连接状态: 断开"));
    }

    if(this->my_database->isOpen()){
        statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态: 连接"));
    }
    else{
        statusLabel2->setText(QString::fromLocal8Bit("数据库连接状态: 断开"));
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

