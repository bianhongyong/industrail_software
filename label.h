#ifndef LABEL_H
#define LABEL_H

#include <QMainWindow>
#include <ConfigManager.h>
#include <global.h>
#include <QListWidgetItem>
#include <manager/annotation_manager.h>
QT_BEGIN_NAMESPACE
namespace Ui { class label; }
QT_END_NAMESPACE

class label : public QMainWindow
{
    Q_OBJECT

public:
    label(QWidget *parent = nullptr);
    ~label();

private slots:
    void on_pushButton_3_clicked();

    void on_not_labeld_clicked();

    void on_labeld_clicked();


    void on_file_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void paint_rect(const QPoint &start,const QPoint & end);
    void update_annotationlist_slot(const QPoint &start,const QPoint & end,const QString & category_idx,const QString & text);
    void not_delete_label_slot(const QString &label_name);
    void paint_update_slot();//从json文件中重新读取信息绘制标注，以及绘制原图
    void paint_original();//将选中的file_list的条目图像重新绘制，避免叠加显示
    void delete_annotation_slot(QListWidgetItem * item);//删除一个标注信息
    void delete_all_annotations_slot();//删除一张图片里所有的标注信息

    void on_not_labeld_toggled(bool checked);

private:
    Ui::label *ui;
    ConfigManager config;
    QString image_path;

    /*缓存路径加载缓存信息相关函数*/
    void load_label_list();
    void load_annotation_list();
    void load_file_list(bool is_labeled);

    void load_config();//刚打开界面时加载

    /*图像显示以及标注绘制相关函数*/
    QColor get_label_list_currentcolor() const;


    /**/
    void image_convert(QListWidgetItem *previous);//决定文件是否要从要转移
    bool moveFile(const QString &sourcePath, const QString &destinationPath);

protected:
     void closeEvent(QCloseEvent *event) override;

};
#endif // LABEL_H
