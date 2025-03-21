#include<widgets/customListwidget.h>
#include<widgets/Abstract.h>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <ConfigManager.h>
#include <QRandomGenerator>
#include <QtGlobal>
#include <manager/annotation_manager.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

CustomListWidget::CustomListWidget(QWidget *parent):QListWidget(parent),provider(nullptr),colors(QList<QColor>()){
     connect(this, &QListWidget::itemChanged, this, &CustomListWidget::onItemchanged);
     connect(this, &QListWidget::itemDoubleClicked, this, &CustomListWidget::onItemDoubleClicked);
}

CustomListWidget::~CustomListWidget()
{
    delete provider;
}


void CustomListWidget::contextMenuEvent(QContextMenuEvent *event) {
    this->set_MenuProvider();
    QListWidgetItem *item = itemAt(event->pos());

    // 如果没有条目被点击，则不显示菜单
    if (!item) {
       return;
    }

    QMenu* menus = new QMenu(this);
    provider->populateMenu(menus,item,this);
     // 显示菜单
    menus->exec(event->globalPos());
    delete menus;
//    qDebug()<<get_item_color(item).name();
}
void CustomListWidget::set_MenuProvider(){
    QString widget_name = this->objectName();
    if(widget_name == QString("label_list")){
        provider = new Label_ContextMenuProvider();

    }
    else if(widget_name == QString("annotation_list")){
        provider = new Annoation_ContextMenuProvider();
    }
    else if(widget_name == QString("file_list")){
        provider = new file_ContextMenuProvider();
    }
}
QIcon CustomListWidget::createColorIcon(){
    // 预定义的50种高区分度颜色（基于HSL色彩空间均匀分布）
    static const QVector<QColor> predefinedColors = []() {
        QVector<QColor> colors;
        const int hueStep = 360 / 50;  // 色相步长
        const int minSaturation = 180; // 最小饱和度（0-255）
        const int maxSaturation = 230; // 最大饱和度
        const int minLightness = 100;  // 最小亮度（0-255）
        const int maxLightness = 150;  // 最大亮度

        for (int i = 0; i < 50; ++i) {
            // 均匀分布色相（0-359）
            int hue = (i * hueStep) % 360;

            // 加入随机扰动增加变化
            int saturation = QRandomGenerator::global()->bounded(minSaturation, maxSaturation);
            int lightness = QRandomGenerator::global()->bounded(minLightness, maxLightness);

            QColor color;
            color.setHsl(hue, saturation, lightness);
            colors.append(color);
        }
        return colors;
    }();

    // 随机选择一个颜色
    int index = QRandomGenerator::global()->bounded(predefinedColors.size());
    QColor color = predefinedColors[index];

    // 创建图标
    QPixmap pixmap(16, 16);
    pixmap.fill(color);

    return QIcon(pixmap);
}
void CustomListWidget::add_color_item(const QString &item_text){
     QListWidgetItem *item = new QListWidgetItem(createColorIcon(), item_text);
     item->setFlags(item->flags() | Qt::ItemIsEditable);
     this->addItem(item);
     this->setCurrentItem(item);//选中新添加的标签
}
void CustomListWidget::add_color_item(const QString &item_text,const QColor &color){
    QPixmap pixmap(16, 16);
    pixmap.fill(color);
    QListWidgetItem *item = new QListWidgetItem(QIcon(pixmap), item_text);
    this->addItem(item);
    this->setCurrentItem(item);//选中新添加的标签
}
void CustomListWidget::add_color_items(const QStringList &item_texts){
     foreach (const QString &item_text, item_texts) {
         QListWidgetItem *item = new QListWidgetItem(createColorIcon(), item_text);
         item->setFlags(item->flags() | Qt::ItemIsEditable);
         this->addItem(item);
     }
}
void CustomListWidget::add_color_items(const QStringList &item_texts,const QList<QColor> &color_list){

    Q_ASSERT_X(item_texts.count()==color_list.count(),"customListwidgt.cpp","两个列表元素个数不一致");
    for(int i = 0;i<item_texts.count();++i){
        this->add_color_item(item_texts[i],color_list[i]);
    }
}
QColor CustomListWidget::get_item_color(){
    return this->currentItem()->icon().pixmap(16, 16).toImage().pixelColor(0, 0); // 获取颜色
}
QColor CustomListWidget::get_item_color(int i){
    return this->item(i)->icon().pixmap(16, 16).toImage().pixelColor(0, 0); // 获取颜色
}
QList<QColor> CustomListWidget::get_item_colors(){
    QList<QColor> colorList;
    for (int i = 0; i < this->count(); ++i) {
           QColor color = this->item(i)->icon().pixmap(16, 16).toImage().pixelColor(0, 0);
            colorList.append(color);
    }
    return colorList;
}

void CustomListWidget::onItemchanged(QListWidgetItem *item){//json文件记得修改
    if(this->objectName()==QString("label_list")){
        ConfigManager cofig(configFilePath);
        // 获取变更后的新内容
        QString newText = item->text();
        // 获取之前存储的旧内容
        QString oldText = item->data(Qt::UserRole).toString();
        if(oldText!=newText&&newText!=""){
            cofig.setLabel(oldText,newText);
            QJsonArray temp = Annotation_manager::categories;
            for(int i=0;i<temp.count();++i){
                if(oldText==temp[i].toObject()["name"].toString()){
                    QJsonObject category_temp = temp[i].toObject();
                    category_temp["name"] = newText;
                    temp[i] = category_temp;
                    Annotation_manager::categories = temp;
                    break;
                }
            }
            emit paint_update();
        }
    }
}
void CustomListWidget::onItemDoubleClicked(QListWidgetItem *item){
    // 在每个项中存储初始文本作为旧内容
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        item->setData(Qt::UserRole, QVariant(item->text()));
    }
}
