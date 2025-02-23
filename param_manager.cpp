#include "param_manager.h"
#include "./ui_param_manager.h"

param_manager::param_manager(QSqlTableModel *SqlTableModel,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::param_manager)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("模具参数管理"));
    ui->tableView->setModel(SqlTableModel);
}

param_manager::~param_manager()
{
    delete ui;
}

void param_manager::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    QSqlTableModel * sqlmodel = qobject_cast<QSqlTableModel *>(ui->tableView->model());//获取数据库模型
    if(sqlmodel->isDirty()){
        reply = QMessageBox::question(this, QString::fromLocal8Bit("保存修改"),
                                      QString::fromLocal8Bit("是否保存对数据库的修改？"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {
            if (sqlmodel->submitAll()) {
                event->accept();
                emit database_flush();//发送数据库更新信号
            }
            else {
                QMessageBox::warning(this, QString::fromLocal8Bit("保存失败"),
                                     QString::fromLocal8Bit("无法保存对数据库的修改。"));
                event->ignore();
            }
        } else if (reply == QMessageBox::No) {
            sqlmodel->revertAll();
            event->accept();
        } else {
            event->ignore();
        }
    }

}
