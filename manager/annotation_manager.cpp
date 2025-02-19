#include "annotation_manager.h"
#include "ConfigManager.h"
QJsonObject Annotation_manager::share_json;
QJsonObject Annotation_manager::annotations;
QJsonArray Annotation_manager::categories;


Annotation_manager::Annotation_manager()
{

}
Annotation_manager::Annotation_manager(const QString &filepath)
{
    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    // 读取文件内容
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (doc.isObject()) {
         Annotation_manager::share_json = doc.object();
         Annotation_manager::annotations = Annotation_manager::share_json["annotations"].toObject();
         Annotation_manager::categories = Annotation_manager::share_json["categories"].toArray();
         //qDebug() << "JSON object loaded successfully:" << share_json;
    } else {
        //qDebug() << "JSON data is not an object.";
    }
}
void Annotation_manager::add_category(const QString &label_name,const int & id){
    QJsonArray categories_temp = categories;
    QJsonObject category;
    category["id"] = id;
    category["name"] = label_name;
    category["count"] = 0;
    categories_temp.append(category);
    Annotation_manager::categories = categories_temp;
    //qDebug()<<categories;
}
void Annotation_manager::delete_category(const int &id){//TODO,如果还存在标注信息则不允许删除
    QJsonArray categories_temp = categories;
    if(categories_temp[id].toObject()["count"]==0){
        categories_temp.removeAt(id);
        for (int i = id; i < categories_temp.size(); ++i) {
            QJsonObject category=categories_temp[i].toObject();
            category["id"] = i;
            categories_temp[i] = category;
        }
        Annotation_manager::categories = categories_temp;
        //qDebug()<<categories;
    }
    else{
        //qDebug()<<"该类别还存在标注无法删除";
    }
    //qDebug()<<share_json;
}
void Annotation_manager::save_json(){
    Annotation_manager::share_json["annotations"] = Annotation_manager::annotations;
    Annotation_manager::share_json["categories"] = Annotation_manager::categories;
    QJsonDocument doc(share_json);
    QByteArray jsonData = doc.toJson();
    QFile file(ConfigManager::getjsonPath());
    file.open(QIODevice::WriteOnly);
    file.write(jsonData);
    file.close();
}
void Annotation_manager::add_annotation(const QString &image_name, QPoint &top_left, QPoint &bottom_right,int  id){
    QJsonObject annotation;
    QJsonArray bbox={top_left.x(),top_left.y(),bottom_right.x(),bottom_right.y()};
    annotation["bbox"] = bbox;
    annotation["category_id"] = id;

    QJsonArray temp;
    if(!annotations.contains(image_name)){
        temp.append(annotation);
    }
    else{
        temp = annotations[image_name].toArray();
        temp.append(annotation);
    }
    annotations[image_name] = temp;

    QJsonArray catagory_temp = categories;
    for(int i = 0;i<catagory_temp.count();i++){
        if(i==id){
            Annotation_manager::category_increase(i);
            break;
        }
    }
}
void Annotation_manager::delete_all_annotations(const QString &image_name){
    /*更正标注类别数量*/
    QJsonArray annotation_temp = Annotation_manager::annotations[image_name].toArray();
    for(int i = 0;i<annotation_temp.count();++i){
        int category_id = annotation_temp[i].toObject()["category_id"].toInt();
        Annotation_manager::category_down(category_id);
    }
    /*删除标注信息*/
    Annotation_manager::annotations.remove(image_name);
}
void Annotation_manager::delete_annotation(const QString &image_name,int annotation_image_index){
    int categary_id = Annotation_manager::annotations[image_name].toArray()[annotation_image_index].toObject()["category_id"].toInt();
    Annotation_manager::category_down(categary_id);
    /*Annotations修改 删除对应的标注信息*/
    QJsonArray annotation_image_list = Annotation_manager::annotations[image_name].toArray();
    annotation_image_list.removeAt(annotation_image_index);
    Annotation_manager::annotations[image_name] = annotation_image_list;
    if(annotation_image_index==0){//删除完最后一个了，别忘记把整个都删除
        Annotation_manager::annotations.remove(image_name);
    }
}

void Annotation_manager::category_increase(int categary_id){
    QJsonObject category_item = Annotation_manager::categories[categary_id].toObject();
    category_item["count"] = category_item["count"].toInt()+1;
    Annotation_manager::categories[categary_id] = category_item;
}
void Annotation_manager::category_down(int categary_id){
    QJsonObject category_item = Annotation_manager::categories[categary_id].toObject();
    category_item["count"] = category_item["count"].toInt()-1;
    Annotation_manager::categories[categary_id] = category_item;
}
