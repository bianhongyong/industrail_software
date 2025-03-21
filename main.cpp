#include "label.h"
#include "collect.h"
#include <QApplication>
#include <QCameraInfo>
#include <QVideoWidget>
class MyApplication : public QApplication {
public:
    using QApplication::QApplication;

    bool notify(QObject* receiver, QEvent* event) override {
        try {
            return QApplication::notify(receiver, event);
        } catch (const std::exception& e) {
            // 在这里处理异常
            qCritical() << "Exception caught:" << e.what();
            saveImportantData();  // 崩溃前保存数据
            QCoreApplication::exit(EXIT_FAILURE);
            // 可以在这里选择退出程序或尝试恢复
            return false;
        } catch (...) {
            qCritical() << "Unknown exception caught";
            saveImportantData();
            QCoreApplication::exit(EXIT_FAILURE);
            return false;
        }
    }

private:
    void saveImportantData() {
        Annotation_manager::save_json();
    }
};

int main(int argc, char *argv[])
{
    // 获取当前程序运行所在的文件夹路径
    QString appDirPath = QCoreApplication::applicationDirPath();

    // 确保路径是绝对路径
    QDir dir(appDirPath);
    QString absolutePath = dir.absolutePath();
    configFilePath = absolutePath;

    MyApplication a(argc, argv);
    collect w;
    w.show();
    return a.exec();
}
