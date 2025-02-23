﻿#ifndef LABEL_H
#define LABEL_H

#include <QMainWindow>
#include <ConfigManager.h>
#include <global.h>
#include <QListWidgetItem>
#include <manager/annotation_manager.h>
#include <QTimer>  // 添加 QTimer 头文件

QT_BEGIN_NAMESPACE
namespace Ui { class label; }
QT_END_NAMESPACE

class label : public QMainWindow
{
    Q_OBJECT

public:
    label(QWidget *parent = nullptr);
    ~label();
    ConfigManager config;
    void load_file_list(bool is_labeled);
private slots:
    void on_pushButton_3_clicked();
    void on_not_labeld_clicked();
    void on_labeld_clicked();
    void on_file_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void paint_rect(const QPoint &start,const QPoint & end);
    void update_annotationlist_slot(const QPoint &start,const QPoint & end,const QString & category_idx,const QString & text);
    void not_delete_label_slot(const QString &label_name);
    void paint_update_slot();
    void paint_original();
    void delete_annotation_slot(QListWidgetItem * item);
    void delete_all_annotations_slot();
    void on_not_labeld_toggled(bool checked);

private:
    Ui::label *ui;
    QString image_path;

    void load_label_list();
    void load_annotation_list();

    void load_config();
    QColor get_label_list_currentcolor() const;
    void image_convert(QListWidgetItem *previous);
    bool moveFile(const QString &sourcePath, const QString &destinationPath);

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void close();
};

#endif // LABEL_H
