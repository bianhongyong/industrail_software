#pragma once

#include <QTableView>
#include <QSqlTableModel>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMenu>
#include <QHeaderView>
#include <QtDebug>
class CustomTabView : public QTableView {
    Q_OBJECT

public:
    explicit CustomTabView(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void addRow();
    void deleteRow();

};
