#include "collect.h"
#include "./ui_collect.h"
#include <QMessageBox>
#include <QStandardPaths>
#include <QUuid>
collect::collect(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::collect),
    Camera(nullptr),
    Cameralist(new QList<QCameraInfo>),
    deviceCheckTimer(new QTimer(this)),
    lastCameraCount(0),
    imageCapture(nullptr)
{
    ui->setupUi(this);
    /*准备工作*/
    getCameras();

    lastCameraCount = Cameralist->count();
    //qDebug()<<lastCameraCount;
    // 设置定时器
    deviceCheckTimer->setInterval(500);  // 每秒检查一次
    connect(deviceCheckTimer, &QTimer::timeout, this, &collect::checkCameraDevices);
    deviceCheckTimer->start();
    imageCapture = new QCameraImageCapture(Camera, this);
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &collect::onImageCaptured);
}

collect::~collect()
{
    delete ui;
    delete Cameralist;
    if(deviceCheckTimer->isActive()) {
        deviceCheckTimer->stop();
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

void collect::startCamera()
{

}

void collect::on_comboBox_currentIndexChanged(const QString &arg1)
{
    //qDebug()<<Camera;
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
        statusBar()->showMessage(QString::fromLocal8Bit("显微摄像头连接状态: 连接"));
    }
    else{
        statusBar()->showMessage(QString::fromLocal8Bit("显微摄像头连接状态: 断开"));
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

void collect::on_shot_clicked()
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

void collect::onImageCaptured(int id, const QImage &preview)
{
    //qDebug()<<imageCapture->isReadyForCapture();
    Q_UNUSED(id);
    // 生成随机文件名
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + QUuid::createUuid().toString() + ".jpg";
    if (preview.save(filePath)) {
        QMessageBox::information(this, QString::fromLocal8Bit("拍照成功"), QString::fromLocal8Bit("图片已保存到: ") + filePath);
    } else {
        QMessageBox::warning(this, QString::fromLocal8Bit("拍照失败"), QString::fromLocal8Bit("图片保存失败"));
    }
}

