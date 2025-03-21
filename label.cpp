﻿#include "label.h"
#include "./ui_label.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
label::label(QWidget *parent)
    : QMainWindow(parent),
    config(QString(configFilePath)),
    ui(new Ui::label)// 初始化定时器

{
    ui->setupUi(this);

    /***加载标签、标注信息***/
    load_config();
    /***连接信号与槽****/
    QObject::connect(ui->actionNext_Image,&QAction::triggered, [this](){
        if(ui->file_list->currentRow()<ui->file_list->count()){
            ui->file_list->setCurrentRow(ui->file_list->currentRow()+1);
        }
    });

    QObject::connect(ui->actionPrevious_Image,&QAction::triggered, [this](){
        if(ui->file_list->currentRow()>0){
            ui->file_list->setCurrentRow(ui->file_list->currentRow()-1);
        }
    });

    QObject::connect(ui->image,&ScalableLabel::paint_signals,this,&label::paint_rect);
    QObject::connect(ui->image,&ScalableLabel::update_all_signal,this,&label::paint_update_slot);
    QObject::connect(ui->label_list,&CustomListWidget::not_delete_label,this,&label::not_delete_label_slot);
    QObject::connect(ui->label_list,&CustomListWidget::paint_update,this,&label::paint_update_slot);
    QObject::connect(ui->annotation_list,&CustomListWidget::delete_annotation,this,&label::delete_annotation_slot);
    QObject::connect(ui->file_list,&CustomListWidget::delete_all_annotations,this,&label::delete_all_annotations_slot);
    QObject::connect(ui->file_list,&CustomListWidget::delete_imagefile,this,&label::delete_imagefile_slot);
    QObject::connect(ui->file_list,&CustomListWidget::delete_all_imagefile,this,&label::delete_all_delete_imagefile_slot);
}

label::~label()
{
    delete ui;
}


void label::on_pushButton_3_clicked()
{
     QString label_name = ui->label_name->text();
     if(label_name!="")//不为空
     {
         QStringList labels_names = config.getLabels();
         if (!labels_names.contains(label_name)){//标签不重复
             ui->label_list->add_color_item(label_name);//带颜色的
             config.addLabel(label_name);//保存在配置文件里,下次程序启动时仍然存在
             Annotation_manager::add_category(label_name,ui->label_list->currentRow());
         }
     }
}

void label::load_annotation_list(){
    ui->annotation_list->clear();
    if(ui->label_list->count()!=0&&ui->file_list->count()!=0){//存在标签信息
        QString image_name = ui->file_list->currentItem()->text();
        if(Annotation_manager::annotations.contains(image_name)){//图片存在标注信息

            Q_ASSERT_X(image_name!=0,"label.cpp","一个图片没有标签，但在json中还存在这个键，但值为空，一定是删除标注信息操作不完善，返回去检查");
            if(Annotation_manager::annotations[image_name]!=0){
                QJsonArray annotation_temp = Annotation_manager::annotations[image_name].toArray();
                //qDebug()<<annotation_temp;
                QList<annotation_item> annotation_list;//绘图信息列表
                QStringList texts_temp;//条目显示信息列表
                QList<QColor> color_list;//颜色信息列表
                for(int i = 0;i < annotation_temp.count();++i){
                    /*获取标注信息列表*/
                    QJsonArray bbox = annotation_temp[i].toObject()["bbox"].toArray();//标注坐标
                    QStringList bboxlist;
                    for (int i = 0; i < bbox.size(); ++i) {
                            QJsonValue value = bbox.at(i);
                            Q_ASSERT_X(value.isDouble(),"label.cpp","标注坐标没有以数字存储");
                            bboxlist.append(QString::number(value.toInt()));
                    }
                    int text_id = annotation_temp[i].toObject()["category_id"].toInt();
                    QVariant temp = i;
                    QString category_index = temp.toString();//标注在当前类别的序号


                    QString name = Annotation_manager::categories[text_id].toObject()["name"].toString();
                    //qDebug()<<name;
                    QString result = category_index+QString("-")+name+QString("   [")+bboxlist.join(", ")+QString("]");
                    texts_temp.append(result);
                    /*获取颜色列表*/
                    QColor Color_temp = ui->label_list->get_item_color(text_id);
                    color_list.append(Color_temp);
                    /*绘画信息列表*/
                    QPoint topLeft(bbox[0].toInt(), bbox[1].toInt());
                    QPoint bottomRight(bbox[2].toInt(), bbox[3].toInt());
                    annotation_item annotation_temp(Color_temp,name,image_name,topLeft,bottomRight,text_id,temp.toInt());
                    annotation_list.append(annotation_temp);

                }

                ui->annotation_list->add_color_items(texts_temp,color_list);//添加信息
                ui->image->paint_labels(annotation_list);
            }
        }
    }
}
void label::load_label_list(){
    ui->label_list->clear();
    QStringList label_names = config.getLabels();
    ui->label_list->add_color_items(label_names);//带颜色的
    ui->label_list->setCurrentRow(0);
}
void label::load_file_list(bool is_labeled){
    ui->file_list->clear();

    QString images_path =is_labeled ?
                config.getImagesPath()+QDir::separator()+QString("labeled") : config.getImagesPath()+QDir::separator()+QString("not_labeled");
    QDir directory(images_path);

    QStringList files = directory.entryList(QDir::Files);
    ui->file_list->addItems(files);
    ui->file_list->setCurrentRow(0);
}

bool label::deleteAllFilesInFolder(const QString &folderPath)
{
    QDir dir(folderPath);

    if (!dir.exists()) {
        return false; // 文件夹不存在
    }

    // 获取文件夹中的所有文件和子文件夹
    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files);

    for (const QFileInfo &info : list) {
        if (info.isDir()) {
            // 如果是子文件夹，递归删除其中的文件
            bool success = deleteAllFilesInFolder(info.absoluteFilePath());
            if (!success) {
                return false;
            }
        } else {
            // 删除文件
            QFile file(info.absoluteFilePath());
            if (!file.remove()) {
                return false;
            }
        }
    }

    return true;
}


void label::load_config(){//TODO
    Annotation_manager Annotation_ma(ConfigManager::getjsonPath());//加载json文件到全局变量，共享资源

    load_label_list();//加载标签

    load_file_list(0);//加载文件列表
    load_annotation_list();//加载标注信息列表
}

void label::on_not_labeld_clicked()
{
    load_file_list(0);//重新载入图像列表
    load_annotation_list();//标注重新载入
    if(ui->file_list->count()==0){//如果图像列表无文件,清空显示的图像
        ui->image->clear();
    }
}

void label::on_labeld_clicked()
{
    load_file_list(1);//重新载入图像列表
    load_annotation_list();//标注重新载入
    if(ui->file_list->count()==0){//如果图像列表无文件,清空显示的图像
        ui->image->clear();
    }
}


void label::on_file_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)//TODO，注意加入标注信息后该怎么显示的问题
{
    /*使能或失能下下翻页动作*/
    // 使能或失能上下翻页动作
    int currentRow = ui->file_list->currentRow();
    int itemCount = ui->file_list->count();

    ui->actionPrevious_Image->setEnabled(currentRow > 0);
    ui->actionNext_Image->setEnabled(currentRow < itemCount - 1);

   if(current!=nullptr){//防止file_list下方Radio_button切换(因为会清空列表)时传入current为空指针卡死，如果为空指针则什么也不做
       ui->image->scaleFactor = 1.0;//切换文件了，缩放比列要恢复
       paint_original();
       image_convert(previous);
       load_annotation_list();
   }

}

void label::paint_rect(const QPoint &start,const QPoint & end){
    if(ui->label_list->count()!=0){
        QColor color = ui->label_list->currentItem()->icon().pixmap(16, 16).toImage().pixelColor(0, 0);//获取选中的当前条目的颜色
        QString text = ui->label_list->currentItem()->text();
        QString image_name = ui->file_list->currentItem()->text();
        int id = ui->label_list->currentRow();
        annotation_item temp(color,text,image_name,id);
        ui->image->paint_rect(start,end,temp);
    }
}
void label::update_annotationlist_slot(const QPoint &start,const QPoint & end,const QString & category_idx,const QString & text){
    QStringList pointsList;
    pointsList.append(QString::number(start.x()));
    pointsList.append(QString::number(start.y()));
    pointsList.append(QString::number(end.x()));
    pointsList.append(QString::number(end.y()));
    // 将列表转换为字符串

    QString result = category_idx+QString("-")+text+QString("   [")+pointsList.join(", ")+QString("]");
    QColor color = ui->label_list->get_item_color();
    ui->annotation_list->add_color_item(result,color);
}
void label::not_delete_label_slot(const QString &label_name){
    QMessageBox msgBox;
    msgBox.setText(label_name+QString::fromLocal8Bit("标签存在对应的标注，无法删除"));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Close); // 不显示任何按钮
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.exec();
}
void label::delete_all_annotations_slot(){//TODO
    // 创建一个 QMessageBox
    QMessageBox msgBox;
    msgBox.setText(QString::fromLocal8Bit("确认要删除")+ui->file_list->currentItem()->text()+QString::fromLocal8Bit("文件对应的所有标注信息?"));
    msgBox.setIcon(QMessageBox::Question); // 设置图标为问号
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No); // 添加“是”和“否”按钮
    msgBox.setDefaultButton(QMessageBox::No); // 设置默认按钮为“否”
    int result = msgBox.exec();
    if(result == QMessageBox::No){
        return;
    }
    else{
        Annotation_manager::delete_all_annotations(ui->file_list->currentItem()->text());
        paint_update_slot();
    }

}
void label::closeEvent(QCloseEvent *event){
    image_convert(ui->file_list->currentItem());
    Annotation_manager::save_json();
    event->accept();
    emit this->close();
}
void label::paint_update_slot(){
    paint_original();
    load_annotation_list();
}
void label::paint_original(){
    QString image_current_path;
    if(ui->file_list->count()>0){
        if(ui->labeld->isChecked()){
            image_current_path=config.getImagesPath()+QDir::separator()+QString("labeled")+QDir::separator()+ui->file_list->currentItem()->text();
        }
        else{
            image_current_path=config.getImagesPath()+QDir::separator()+QString("not_labeled")+QDir::separator()+ui->file_list->currentItem()->text();
        }

        //qDebug()<<image_current_path;
        if(!imageCache.contains(image_current_path)){
            if(imageCache.size()>50){//缓冲区最多存50副图像
                QString firstKey = imageCache.firstKey();
                imageCache.remove(firstKey);
            }
            QPixmap pixmap;
            pixmap.load(image_current_path);
            pixmap = pixmap.scaled(640,640);
            imageCache.insert(image_current_path,pixmap);
        }
        // QPixmap pixmap;
        // pixmap.load(image_current_path);
        // pixmap = pixmap.scaled(640,640);

        ui->image->setPixmap(imageCache[image_current_path]);
        ui->image->originalPixmap = imageCache[image_current_path];
        ui->image->updatePixmap();
    }

}
void label::delete_annotation_slot(QListWidgetItem * item){
    // 创建一个 QMessageBox
    QMessageBox msgBox;
    msgBox.setText(QString::fromLocal8Bit("确认要删除")+item->text()+QString::fromLocal8Bit("  这条标注信息?"));
    msgBox.setIcon(QMessageBox::Question); // 设置图标为问号
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No); // 添加“是”和“否”按钮
    msgBox.setDefaultButton(QMessageBox::No); // 设置默认按钮为“否”
    int result = msgBox.exec();
    if(result == QMessageBox::No){
        return;
    }
    else{
        QString images_names = ui->file_list->currentItem()->text();
        int annotation_image_index = QVariant(ui->annotation_list->currentRow()).toInt();
        Annotation_manager::delete_annotation(images_names,annotation_image_index);
        paint_update_slot();
        ui->annotation_list->setCurrentRow(annotation_image_index);
    }
}
void label::image_convert(QListWidgetItem *previous){
    if(previous!=nullptr){
        if(ui->not_labeld->isChecked()){//未标注按钮选中
            QString image_name = previous->text();
            if(Annotation_manager::annotations.contains(image_name)){//存在标记了
                QString sourcePath = config.getImagesPath()+QDir::separator()+QString("not_labeled")+QDir::separator()+previous->text();
                QString destinationPath = config.getImagesPath()+QDir::separator()+QString("labeled")+QDir::separator()+previous->text();
                this->moveFile(sourcePath,destinationPath);
                load_file_list(0);
            }
        }
        else{//标注按钮选中
            QString image_name = previous->text();
            if(!Annotation_manager::annotations.contains(image_name)){//不存在标记了
                QString destinationPath = config.getImagesPath()+QDir::separator()+QString("not_labeled")+QDir::separator()+previous->text();
                QString sourcePath = config.getImagesPath()+QDir::separator()+QString("labeled")+QDir::separator()+previous->text();
                this->moveFile(sourcePath,destinationPath);
                load_file_list(1);
            }
        }
    }

}

bool label::moveFile(const QString &sourcePath, const QString &destinationPath)
{
    QFile file(sourcePath);
    if (!file.rename(destinationPath)) {
        qDebug() << "Failed to move file:" << sourcePath;
        return false;
    }
    return true;
}

void label::on_not_labeld_toggled(bool checked)
{

}

void label::delete_imagefile_slot(QListWidgetItem *item)
{
    if(Annotation_manager::annotations.contains(item->text())){
        QMessageBox msgBox;
        msgBox.setText(item->text()+QString::fromLocal8Bit("图像存在标注，无法删除该图像"));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Close); // 不显示任何按钮
        msgBox.setDefaultButton(QMessageBox::Close);
        msgBox.exec();
    }
    else{
        QString image_path;
        if(ui->not_labeld->isChecked()){
            image_path = config.getImagesPath()+QDir::separator()+QString("not_labeled")+QDir::separator()+item->text();
        }
        else{
            image_path = config.getImagesPath()+QDir::separator()+QString("labeled")+QDir::separator()+item->text();
        }
        QFile file(image_path);
        if(file.remove()){
            int index = ui->file_list->row(item);

            load_file_list(ui->labeld->isChecked());
            ui->image->clear();
            if(index>0){
                ui->file_list->setCurrentRow(index-1);
            }
            paint_original();
        }
    }
}
void label::copyRecursively(const QString &srcPath, const QString &destPath)
{
    QDir srcDir(srcPath);
    if (!srcDir.exists())
        return;

    QDir destDir(destPath);
    if (!destDir.exists())
        destDir.mkpath(destPath);

    foreach (QString fileName, srcDir.entryList(QDir::Files))
    {
        QString srcFilePath = srcPath + QDir::separator() + fileName;
        QString destFilePath = destPath + QDir::separator() + fileName;
        QFile::copy(srcFilePath, destFilePath);
    }

    foreach (QString dirName, srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString srcDirPath = srcPath + QDir::separator() + dirName;
        QString destDirPath = destPath + QDir::separator() + dirName;
        copyRecursively(srcDirPath, destDirPath);
    }
}

void label::on_action_save_triggered()
{
    QString exportPath = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择导出路径"), QDir::homePath());
    if (exportPath.isEmpty()) {
        return;
    }
    QString tempDirPath = QDir::tempPath() + QDir::separator() + "export_temp";
    QDir tempDir(tempDirPath);

    if (tempDir.exists()) {
        tempDir.removeRecursively();
    }
    tempDir.mkpath(tempDirPath);
    // 复制整个图像文件夹到临时目录
    QString imagesPath = config.getImagesPath();
    QString destImagesPath = tempDirPath + QDir::separator() + "images";
    copyRecursively(imagesPath, destImagesPath);

    // 复制 JSON 文件到临时目录
    QString jsonFilePath = ConfigManager::getjsonPath();
    QString destJsonPath = tempDirPath + QDir::separator() + "label.json";
    QFile::copy(jsonFilePath, destJsonPath);

    copyRecursively(tempDirPath, exportPath);

    // 删除临时目录
    tempDir.removeRecursively();

    QMessageBox::information(this, QString::fromLocal8Bit("导出成功"), QString::fromLocal8Bit("导出成功"));
}
void label::delete_all_delete_imagefile_slot()
{

    if(ui->not_labeld->isChecked()){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(QString::fromLocal8Bit("确认删除"));
        msgBox.setText(QString::fromLocal8Bit("你确定要删除列表中的所有文件吗(删除后不可恢复)？如确认删除，请先导出保存"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        // 显示对话框并获取用户选择
        int result = msgBox.exec();
        if (result == QMessageBox::Yes) {
            for(int i = 0;i<ui->file_list->count();++i){
                 QString image_name = ui->file_list->item(i)->text();
                 if(Annotation_manager::annotations.contains(image_name)){
                     QMessageBox::information(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("列表存在标注文件，无法全部删除"));
                     return;
                 }
            }

            bool ifdelete=deleteAllFilesInFolder(configFilePath+QDir::separator()+QString("images")+QDir::separator()+QString("not_labeled"));
            if(ifdelete){
                 QMessageBox::information(this, QString::fromLocal8Bit("删除成功"), QString::fromLocal8Bit("删除成功"));
                 load_file_list(0);
                 ui->image->clear();
            }
            else{
                 QMessageBox::information(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败请重试"));
            }
        }
        else{
            return;
        }
    }
}
