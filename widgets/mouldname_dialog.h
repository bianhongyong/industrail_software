#ifndef MOULDNAME_DIALOG_H
#define MOULDNAME_DIALOG_H

#include <QDialog>

namespace Ui {
class mouldName_dialog;
}

class mouldName_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit mouldName_dialog(QWidget *parent = nullptr);
    ~mouldName_dialog();
protected:
    void closeEvent(QCloseEvent *event) override;
signals:
    void show_collect();
    void start_detect(QString mould_text);
private slots:
    void on_exit_clicked();
    void on_ok_clicked();
private:
    Ui::mouldName_dialog *ui;
};

#endif // MOULDNAME_DIALOG_H
