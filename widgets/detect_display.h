#ifndef DETECT_DISPLAY_H
#define DETECT_DISPLAY_H

#include <QWidget>

namespace Ui {
class detect_display;
}

class detect_display : public QWidget
{
    Q_OBJECT

public:
    explicit detect_display(QWidget *parent = nullptr);
    ~detect_display();
protected:
    void closeEvent(QCloseEvent *event) override;
public slots:
    void change_label(QString text);
private:
    Ui::detect_display *ui;
    QString mouldName;
signals:
    void close_signal();
};

#endif // DETECT_DISPLAY_H
