#include "ConfigManager.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
ConfigManager::ConfigManager(const QString &filePath) {
    /*检测配置文件,默认路径是否存在,不存在按照默认配置创建*/

    //创建根目录
    QDir dir;
    if(!dir.exists(filePath)){
        qDebug()<<filePath;
        bool result = dir.mkpath(filePath);
        if (result) {
            qDebug() << "文件夹创建成功：" << filePath;
        } else {
            qDebug() << "文件夹创建失败：" << filePath;
        }
    }

    //创建配置文件
   if(!QFile::exists(filePath+QDir::separator()+QString("config.ini"))){
        QFile file(filePath+QDir::separator()+QString("config.ini"));
        if (file.open(QIODevice::WriteOnly)) {
                 QTextStream out(&file);
                 file.close();
            }
   }

   //创建图像保存文件夹
   if(!dir.exists(filePath+QDir::separator()+QString("images"))){
        dir.mkpath(filePath+QDir::separator()+QString("images"));
        dir.mkpath(filePath+QDir::separator()+QString("images")+QDir::separator()+QString("not_labeled"));
        dir.mkpath(filePath+QDir::separator()+QString("images")+QDir::separator()+QString("labeled"));
   }
   //json文件创建以及初始化
   if(!QFile::exists(filePath+QDir::separator()+QString("label.json"))){
       QFile file(filePath+QDir::separator()+QString("label.json"));
       if (file.open(QIODevice::WriteOnly)) {//TODO,一般不是空的json文件，后期修改
           // 创建根对象
           QJsonObject root,images;
           // 创建图像列表
           QJsonArray categories;
           root["annotations"] = images;
           root["categories"] = categories;
           // 将 JSON 对象转换为字符串
           QJsonDocument doc(root);
           QByteArray jsonData = doc.toJson();
           file.write(jsonData);
           file.close();
       }
   }

   //config.ini初始化
   settings = new QSettings(filePath+QDir::separator()+QString("config.ini"), QSettings::IniFormat);
   if(!settings->contains(imagesPathKey)){
        settings->setValue(imagesPathKey, filePath+QDir::separator()+QString("images"));
        settings->sync();
    }
    if(!settings->contains(labelsKey)){
        settings->setValue(labelsKey,QStringList());
        settings->sync();
    }
}

void ConfigManager::setImagesPath(const QString &path) {
    settings->setValue(imagesPathKey, path);
}

QString ConfigManager::getImagesPath() const {
    return settings->value(imagesPathKey).toString();
}

void ConfigManager::addLabel(const QString &label) {
    QStringList labels = getLabels();
    labels.append(label);
    settings->setValue(labelsKey, labels);

}

QList<QString> ConfigManager::getLabels() const {
    return settings->value(labelsKey).toStringList();
}

void ConfigManager::removeLabel(const QString &label) {
    QStringList labels = getLabels();
    labels.removeAll(label);
    settings->setValue(labelsKey, labels);
}
void ConfigManager::removeLabel(int index) {
    QStringList labels = getLabels();
    if (index >= 0 && index < labels.size()) {
           labels.removeAt(index);
    }
    settings->setValue(labelsKey, labels);

}

void ConfigManager::setLabel(const QString &old_label,const QString &new_label){
    QStringList labels = getLabels();
    for(int i = 0;i<labels.count();i++){
        if(labels[i]==old_label){
            labels[i] = new_label;
        }
    }
    settings->setValue(labelsKey, labels);
}
QString ConfigManager::getjsonPath(){
    return configFilePath+QDir::separator()+QString("label.json");
}
ConfigManager::~ConfigManager(){
    delete settings;
}
