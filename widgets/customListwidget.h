#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H
#include<QListWidget>
#include <global.h>

class IContextMenuProvider;//前向声明，解决循环依赖问题

class CustomListWidget : public QListWidget {
    Q_OBJECT
    static QIcon createColorIcon();
public:
    explicit CustomListWidget(QWidget *parent = nullptr);
    ~CustomListWidget();
    void add_color_item(const QString &item_text);//随机获取颜色
    void add_color_item(const QString &item_text,const QColor &color);//指定颜色


    void add_color_items(const QStringList &item_texts);
    void add_color_items(const QStringList &item_texts,const QList<QColor> &color_list);


    QColor get_item_color();//获取条目前面标注的颜色
    QColor get_item_color(int id);//获取条目前面标注的颜色,重载
    QList<QColor> get_item_colors();//获取所有条目前面标注的颜色
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
private:
    IContextMenuProvider* provider;
    void set_MenuProvider();
    QList<QColor> colors;
protected slots:
    void onItemchanged(QListWidgetItem *item);
    void onItemDoubleClicked(QListWidgetItem *item);

signals:
    void not_delete_label(const QString &label_name);
    void paint_update();//重绘信号
    void delete_annotation(QListWidgetItem *item);//删除一个标注信号
    void delete_all_annotations();//删除一张图片所有标注信号
    void delete_imagefile(QListWidgetItem *item);
};

#endif // CUSTOMLISTWIDGET_H
