#include<widgets/Abstract.h>
#include<widgets/customListwidget.h>
#include<ConfigManager.h>
#include<manager/annotation_manager.h>
void Annoation_ContextMenuProvider::populateMenu(QMenu *menu, QListWidgetItem *item, CustomListWidget *one_list){
    QAction *action1 = new QAction(QString::fromLocal8Bit("删除选中标注"), menu);
    QObject::connect(action1, &QAction::triggered, [item,one_list]() {
        emit one_list->delete_annotation(item);
    });

    menu->addAction(action1);
}
void file_ContextMenuProvider::populateMenu(QMenu *menu, QListWidgetItem *item, CustomListWidget *one_list){
    QAction *action1 = new QAction(QString::fromLocal8Bit("删除选中文件"), menu);
    //throw std::runtime_error("测试异常：用户手动触发的崩溃"); 人为构造异常测试
    QObject::connect(action1, &QAction::triggered, [item,one_list](){
        emit one_list->delete_imagefile(item);//CustomListWidget发送信号交给label类处理
    });
    menu->addAction(action1);

    QAction *action2 = new QAction(QString::fromLocal8Bit(("删除文件对应的标注")), menu);
    QObject::connect(action2, &QAction::triggered, [one_list]() {
        emit one_list->delete_all_annotations();//CustomListWidget发送信号交给label类处理
    });
    menu->addAction(action2);

    QAction *action3 = new QAction(QString::fromLocal8Bit(("删除列表所有文件")),menu);
    QObject::connect(action3, &QAction::triggered, [one_list]() {
        emit one_list->delete_all_imagefile();//CustomListWidget发送信号交给label类处理
    });
    menu->addAction(action3);
}
void Label_ContextMenuProvider::populateMenu(QMenu *menu, QListWidgetItem *item,CustomListWidget *one_list){
    QAction *action1 = new QAction(QString::fromLocal8Bit("删除选中标签"), menu);
    QObject::connect(action1, &QAction::triggered, [item,one_list]() {
        ConfigManager settings(configFilePath);
        int index = one_list->row(item);
        if(Annotation_manager::categories[index].toObject()["count"].toInt()==0){
            settings.removeLabel(one_list->currentRow());//配置文件同时删除，保持一致性
            Annotation_manager::delete_category(one_list->currentRow());//json文件同时删除保持一致性
            delete item;//删除该条目
        }
        else{//如果存在标签则不能删除
            emit one_list->not_delete_label(item->text());//触发无法删除信号，label祖先响应
        }
    });

    menu->addAction(action1);

}
