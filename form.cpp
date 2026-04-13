#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);// 设置为非模态
}

Form::~Form()
{
    delete ui;
}

void Form::closeEvent(QCloseEvent *event)
{
    emit this->show_collect();
    ui->lineEdit->clear();
}

void Form::on_exit_clicked()
{
    emit this->show_collect();
}


void Form::on_ok_clicked()
{
    QString mould_id = ui->lineEdit->text();
    emit this->start_detect(mould_id);
}

