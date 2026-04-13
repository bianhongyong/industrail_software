#include "detect_display.h"
#include "./ui_detect_display.h"
#include "global.h"
#include "QDir"
detect_display::detect_display(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::detect_display)
{
    ui->setupUi(this);

    connect(ui->image,&ScalableLabel::update_all_signal,this,[=](){
        QPixmap pixmap;
        pixmap.load(configFilePath+QDir::separator()+QString("detect_cache")+QDir::separator()
                    +QString("20250401")+QDir::separator()+QString::fromLocal8Bit("模具1_20250321133259395.png"));
        pixmap = pixmap.scaled(640,640);
        ui->image->originalPixmap = pixmap;
        ui->image->updatePixmap();
    });
}

detect_display::~detect_display()
{
    delete ui;
}

void detect_display::closeEvent(QCloseEvent *event)
{
    emit this->close_signal();
}

void detect_display::change_label(QString text)
{
    this->ui->name->setText(text);
    mouldName = text;
}
