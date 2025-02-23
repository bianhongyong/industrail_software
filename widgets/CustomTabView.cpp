#include "CustomTabView.h"

CustomTabView::CustomTabView(QWidget *parent)
    : QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void CustomTabView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);

    if (horizontalHeader()->geometry().contains(pos)) {
        return;
    }
    else if(index.isValid()){
        // 行操作菜单项
        QAction *addRowAction = menu.addAction(QString::fromLocal8Bit("新增行"));
        QAction *deleteRowAction = menu.addAction(QString::fromLocal8Bit("删除行"));
        connect(addRowAction, &QAction::triggered, this, &CustomTabView::addRow);
        connect(deleteRowAction, &QAction::triggered, this, &CustomTabView::deleteRow);
    }
    else{
        QAction *addRowAction = menu.addAction(QString::fromLocal8Bit("新增行"));
        connect(addRowAction, &QAction::triggered, this, &CustomTabView::addRow);
    }

    menu.exec(event->globalPos());
}

void CustomTabView::addRow()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    if (model) {
        model->insertRow(model->rowCount());
    }

}

void CustomTabView::deleteRow()
{
    QModelIndexList selected = selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }
    foreach (QModelIndex index, selected) {
        model()->removeRow(index.row());
    }

}


