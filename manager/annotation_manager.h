#ifndef ANNOTATION_MANAGER_H
#define ANNOTATION_MANAGER_H
#include<QString>
#include<QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonParseError>
#include <qdebug.h>
class Annotation_manager
{


public:
    static QJsonObject share_json;//所有实例共享
    static QJsonObject annotations;
    static QJsonArray categories;
    Annotation_manager();
    Annotation_manager(const QString & json_filepath);//载入缓存的json文件


    static void add_category(const QString & label_name,const int & id);//增加新的标签
    static void delete_category(const int & id);//删除标签

    static void save_json();//将share_json对象保存值label.json缓存文件夹
    static void add_annotation(const QString &image_name,QPoint &top_left,QPoint &bottom_right,int id);
    static void delete_annotation(const QString &image_name,int id);
    static void delete_all_annotations(const QString &image_name);
    static void category_increase(int id);
    static void category_down(int id);
};

#endif // ANNOTATION_MANAGER_H
