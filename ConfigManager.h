#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QSettings>
#include <QString>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <global.h>
#include <QDir>

class ConfigManager {

public:

    static QString getjsonPath();


    explicit ConfigManager(const QString &filePath);
    ~ConfigManager();
    // 设置和获取图片存储路径
    void setImagesPath(const QString &path);
    QString getImagesPath() const;

    // 添加、获取和删除用户自定义标签
    void addLabel(const QString &label);
    QList<QString> getLabels() const;
    void removeLabel(const QString &label);
    void removeLabel(int index);
    void setLabel(const QString &old_label,const QString &new_label);
private:
    QSettings *settings;
    QString imagesPathKey = "Storage/ImagesPath";
    QString labelsKey = "Storage/Labels";


};

#endif // CONFIGMANAGER_H
