#ifndef EXTENDEDCOMBOBOX_H
#define EXTENDEDCOMBOBOX_H

#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QKeyEvent>

class ExtendedComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit ExtendedComboBox(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    QSortFilterProxyModel *m_proxyModel;
    QCompleter *m_customCompleter;

    void setupFilter();
    void validateCurrentText();  // 添加 validateCurrentText 方法
};

#endif // EXTENDEDCOMBOBOX_H
