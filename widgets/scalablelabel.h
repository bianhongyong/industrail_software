#ifndef SCALABLELABEL_H
#define SCALABLELABEL_H

#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QWheelEvent>
#include <QPainter>
#include <manager/annotation_manager.h>

struct annotation_item{//将一个标注完成时需要在不同组件之间传递的信息
    QColor color;//标注对应类别的颜色
    QString text;//标注对应类别的名称
    QString image_name;//标注所在的图片名称
    QPoint start_point,end_point;//左上，右下角的点
    int id;//标注对应的序号，也是label_list控件显示标签的顺序，一一对应
    int category_index;//标注在该幅图像下的标注
    annotation_item(const QColor &color,const QString &text,const QString &image_name,int id):color(color),text(text),image_name(image_name),id(id){}
    annotation_item(const QColor &color,const QString &text,const QString &image_name,const QPoint &start_point,const QPoint &end_point,int id,int category_index):
        color(color),text(text),image_name(image_name),start_point(start_point),end_point(end_point),id(id),category_index(category_index){}
};
//标注软件的label类
class ScalableLabel : public QLabel
{
    Q_OBJECT
public:
    ScalableLabel(QWidget *parent = nullptr);
    QPixmap originalPixmap;
    double scaleFactor;
    void updatePixmap();
    void paint_rect(const QPoint &start,const QPoint & end,const annotation_item info);
    void paint_realtime(const QPoint &start, const QPoint &end,const annotation_item info);//标注时的实时绘画
    void paint_labels(const QList<annotation_item>& item);//绘画一个标注信息

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    bool isDrawing = false;
    bool isDrawing_save = false;
    QPoint rectStart, rectEnd;
    QPoint labelToPixmapCoords(const QPoint &labelPos);

signals:
    void paint_signals(const QPoint &start,const QPoint & end);//绘制信号:主要向label父亲请求信息传回处理
    void update_all_signal();
};
//缺陷显示的label类，去除绘画功能
class ReadOnlyScalableLabel : public ScalableLabel
{
    Q_OBJECT
public:
    using ScalableLabel::ScalableLabel;

protected:
    // 保留滚轮缩放功能
    void wheelEvent(QWheelEvent *event) override {
        ScalableLabel::wheelEvent(event);
    }
    // 禁用鼠标交互
    void mousePressEvent(QMouseEvent *event) override {
        event->ignore();  // 忽略鼠标事件
        // 或者直接空实现不处理
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        event->ignore();
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        event->ignore();
    }
    // 禁用自定义绘制
    void paintEvent(QPaintEvent *event) override {
        // 仅保留基础QLabel的绘制逻辑
        QLabel::paintEvent(event);
    }
};

#endif // SCALABLELABEL_H
