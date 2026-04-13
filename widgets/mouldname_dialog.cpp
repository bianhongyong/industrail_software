#include "mouldname_dialog.h"
#include "ui_mouldname_dialog.h"

mouldName_dialog::mouldName_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mouldName_dialog)
{
    ui->setupUi(this);
    this->setModal(true);
}

mouldName_dialog::~mouldName_dialog()
{
    delete ui;
}

void mouldName_dialog::closeEvent(QCloseEvent *event)
{
    emit this->show_collect();
    ui->lineEdit->clear();
}

void mouldName_dialog::on_exit_clicked()
{
    emit this->show_collect();
}

void mouldName_dialog::on_ok_clicked()
{
    QString mould_id = ui->lineEdit->text();
    emit this->start_detect(mould_id);
}

