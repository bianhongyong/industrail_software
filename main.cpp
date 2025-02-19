#include "label.h"
#include "collect.h"
#include <QApplication>
#include <QCameraInfo>
#include <QVideoWidget>



int main(int argc, char *argv[])
{
    // 获取当前程序运行所在的文件夹路径
    QString appDirPath = QCoreApplication::applicationDirPath();

    // 确保路径是绝对路径
    QDir dir(appDirPath);
    QString absolutePath = dir.absolutePath();
    configFilePath = absolutePath;

    QApplication a(argc, argv);
    collect w;
    w.show();
    return a.exec();
}
