#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QDebug>
namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::Form *ui;
signals:
    void show_collect();
    void start_detect(QString mould_text);
private slots:
    void on_exit_clicked();
    void on_ok_clicked();
};

#endif // FORM_H
