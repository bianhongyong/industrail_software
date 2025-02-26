#include "scalablelabel.h"
#include "qdebug.h"


ScalableLabel::ScalableLabel(QWidget *parent):QLabel(parent),scaleFactor(1.0){}

void ScalableLabel::wheelEvent(QWheelEvent *event){ //有点问题,图像失真问题
    // 计算缩放比例
    if(event->modifiers() & Qt::ControlModifier ){
      if(!isDrawing&&!isDrawing_save){
          const double degrees = event->angleDelta().y() / 8.0;
          const double step = (degrees > 0) ? 1.1 : 0.9;
          scaleFactor *= step;
          scaleFactor = qBound(0.2, scaleFactor, 3.0); // 限制缩放范围
          //更新图像
          emit update_all_signal();
      }
    }
    event->accept();
}

void ScalableLabel::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton) {
        //qDebug()<<"1";
        // 开始绘制
        if(!isDrawing&&this->pixmap()!=nullptr){
            isDrawing = true;
            rectStart = event->pos();
            rectEnd = rectStart;
        }
    }
    event->accept();
}

void ScalableLabel::mouseMoveEvent(QMouseEvent *event){
    if (isDrawing) {
        rectEnd = event->pos();
       // qDebug() << "Rectangle drawn from" << rectEnd;
        update(); // 触发重绘
    }
    event->accept();
}

void ScalableLabel::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
    {
        //qDebug()<<"2";
        isDrawing = false;
        isDrawing_save = true;
        if(rectStart!=rectEnd) update(); //起点和重点不重合
        else{
            isDrawing_save = false;
        }
    }
    event->accept();
}

void ScalableLabel::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    if(isDrawing||isDrawing_save) emit paint_signals(rectStart,rectEnd);//交给label类完成，因为需要其他组件的信息
}

void ScalableLabel::updatePixmap(){
    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    this->setPixmap(scaledPixmap);
}
QPoint ScalableLabel::labelToPixmapCoords(const QPoint &labelPos){
    QRect pixmapRect = (*this->pixmap()).rect();
    QRect labelRect = this->rect();
    double x_start = (labelRect.width()-pixmapRect.width())/2.0;
    double y_start = (labelRect.height()-pixmapRect.height())/2.0;
    // 转换坐标
    QPoint pixmapPos(
                static_cast<int>((labelPos.x() - x_start)),
                static_cast<int>((labelPos.y() - y_start))
                );
     //qDebug()<<pixmapPos;
     return pixmapPos;
}

void ScalableLabel::paint_rect(const QPoint &start, const QPoint &end,const annotation_item info){//TODO

    if (isDrawing) {//动态显示
        paint_realtime(start,end,info);
    }
    if(isDrawing_save){//保存在pixmap上，并同步更新标注文件信息
        isDrawing_save=false;
        QPixmap current = *(this->pixmap());
        QPainter painter(&current);
        painter.setPen(QPen(info.color, 3*scaleFactor));
        QFont font("Arial", 12*scaleFactor, QFont::Bold);
        painter.setFont(font);

        QPoint Pixmap_start = this->labelToPixmapCoords(rectStart);
        QPoint Pixmap_end = this->labelToPixmapCoords(rectEnd);

        /*标注信息同步修改*/
        QPoint startpoint_original_size,endpoint_original_size;//坐标转换
        startpoint_original_size.setX(Pixmap_start.x()/scaleFactor);
        startpoint_original_size.setY(Pixmap_start.y()/scaleFactor);
        endpoint_original_size.setX(Pixmap_end.x()/scaleFactor);
        endpoint_original_size.setY(Pixmap_end.y()/scaleFactor);
        Annotation_manager::add_annotation(info.image_name,startpoint_original_size,endpoint_original_size,info.id);



        /*更新AnnotationList列表以及重绘坐标*/
        emit update_all_signal();
    }
}
void ScalableLabel::paint_realtime(const QPoint &start, const QPoint &end,const annotation_item info){
    QPainter painter(this);
    painter.setPen(QPen(info.color, 3*scaleFactor));
    // 计算矩形框的起点和终点
    QRect rect(rectStart, rectEnd);
    rect = rect.normalized();
    // 绘制矩形框
    painter.drawRect(rect);
}
void ScalableLabel::paint_labels(const QList<annotation_item>& item){//TODO,可以进行优化，现在绘制大量矩形框很卡
    if(this->pixmap()!=nullptr){//我也不知道为什么,必须加这个，可能某个事件触发时机我没有考虑到位
        QPixmap current = *(this->pixmap());
        QPainter painter(&current);
        painter.setRenderHint(QPainter::Antialiasing);


        //绘制离屏图像
        QImage offscreenImage(current.size(), QImage::Format_ARGB32_Premultiplied);
        offscreenImage.fill(Qt::transparent);

        QPainter offscreenPainter(&offscreenImage);
        offscreenPainter.setRenderHint(QPainter::Antialiasing);
        QFont font("Arial", 12*scaleFactor, QFont::Bold);
        offscreenPainter.setFont(font);

        for(int i = 0;i<item.count();++i){
            annotation_item annotation_temp = item[i];
            offscreenPainter.setPen(QPen(annotation_temp.color, 3*scaleFactor));
            QPoint start_point(annotation_temp.start_point.x()*scaleFactor,annotation_temp.start_point.y()*scaleFactor);
            QPoint end_point(annotation_temp.end_point.x()*scaleFactor,annotation_temp.end_point.y()*scaleFactor);
            QRect rect(start_point, end_point);
            offscreenPainter.drawRect(rect);
            QRect textRect(rect.left(), rect.top() - 20*scaleFactor, 300*scaleFactor, 20*scaleFactor);
            QString content =  QVariant(annotation_temp.category_index).toString()+QString("-")+annotation_temp.text;
            offscreenPainter.drawText(textRect, Qt::AlignLeft,content);
        }
        painter.drawImage(0, 0, offscreenImage);
        this->setPixmap(current);
        originalPixmap = current.scaled(640,640);
    }
}
