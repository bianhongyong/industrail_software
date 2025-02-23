#ifndef PARAM_MANAGER_H
#define PARAM_MANAGER_H

#include <QWidget>
#include <QSqlTableModel>
#include <qdebug.h>
namespace Ui {
class param_manager;
}

class param_manager : public QWidget
{
    Q_OBJECT

public:
    explicit param_manager(QSqlTableModel* table_model,QWidget *parent = nullptr);
    ~param_manager();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::param_manager *ui;

signals:
    void database_flush();
};

#endif // PARAM_MANAGER_H
