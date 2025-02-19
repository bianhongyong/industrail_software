/********************************************************************************
** Form generated from reading UI file 'label.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LABEL_H
#define UI_LABEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <widgets/Scalablelabel.h>
#include <widgets/customlistwidget.h>

QT_BEGIN_NAMESPACE

class Ui_label
{
public:
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionZoom_in;
    QAction *actionZoom_out;
    QAction *actionPrevious_Image;
    QAction *actionNext_Image;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_5;
    ScalableLabel *image;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QDockWidget *annotationWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_3;
    CustomListWidget *annotation_list;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QDockWidget *labelWidget;
    QWidget *dockWidgetContents_5;
    QVBoxLayout *verticalLayout_2;
    CustomListWidget *label_list;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *label_name;
    QPushButton *pushButton_3;
    QDockWidget *fileWidget;
    QWidget *dockWidgetContents_2;
    QVBoxLayout *verticalLayout_4;
    CustomListWidget *file_list;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *not_labeld;
    QRadioButton *labeld;

    void setupUi(QMainWindow *label)
    {
        if (label->objectName().isEmpty())
            label->setObjectName(QString::fromUtf8("label"));
        label->resize(1006, 853);
        actionUndo = new QAction(label);
        actionUndo->setObjectName(QString::fromUtf8("actionUndo"));
        actionUndo->setCheckable(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/icons/undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUndo->setIcon(icon);
        actionRedo = new QAction(label);
        actionRedo->setObjectName(QString::fromUtf8("actionRedo"));
        actionRedo->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/redo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRedo->setIcon(icon1);
        actionZoom_in = new QAction(label);
        actionZoom_in->setObjectName(QString::fromUtf8("actionZoom_in"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_in->setIcon(icon2);
        actionZoom_out = new QAction(label);
        actionZoom_out->setObjectName(QString::fromUtf8("actionZoom_out"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/icons/zoom-out.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_out->setIcon(icon3);
        actionPrevious_Image = new QAction(label);
        actionPrevious_Image->setObjectName(QString::fromUtf8("actionPrevious_Image"));
        actionPrevious_Image->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/icons/prev.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrevious_Image->setIcon(icon4);
        actionNext_Image = new QAction(label);
        actionNext_Image->setObjectName(QString::fromUtf8("actionNext_Image"));
        actionNext_Image->setEnabled(true);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/icons/next.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNext_Image->setIcon(icon5);
        centralwidget = new QWidget(label);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollArea->setAlignment(Qt::AlignCenter);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 711, 751));
        verticalLayout_5 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        image = new ScalableLabel(scrollAreaWidgetContents);
        image->setObjectName(QString::fromUtf8("image"));
        image->setScaledContents(false);
        image->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(image);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        label->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(label);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        label->setStatusBar(statusbar);
        toolBar = new QToolBar(label);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        label->addToolBar(Qt::TopToolBarArea, toolBar);
        menuBar = new QMenuBar(label);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1006, 21));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu->setToolTipsVisible(false);
        label->setMenuBar(menuBar);
        annotationWidget = new QDockWidget(label);
        annotationWidget->setObjectName(QString::fromUtf8("annotationWidget"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout_3 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        annotation_list = new CustomListWidget(dockWidgetContents);
        annotation_list->setObjectName(QString::fromUtf8("annotation_list"));

        verticalLayout_3->addWidget(annotation_list);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton = new QPushButton(dockWidgetContents);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(dockWidgetContents);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);


        verticalLayout_3->addLayout(horizontalLayout);

        annotationWidget->setWidget(dockWidgetContents);
        label->addDockWidget(static_cast<Qt::DockWidgetArea>(2), annotationWidget);
        labelWidget = new QDockWidget(label);
        labelWidget->setObjectName(QString::fromUtf8("labelWidget"));
        dockWidgetContents_5 = new QWidget();
        dockWidgetContents_5->setObjectName(QString::fromUtf8("dockWidgetContents_5"));
        verticalLayout_2 = new QVBoxLayout(dockWidgetContents_5);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_list = new CustomListWidget(dockWidgetContents_5);
        label_list->setObjectName(QString::fromUtf8("label_list"));

        verticalLayout_2->addWidget(label_list);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_name = new QLineEdit(dockWidgetContents_5);
        label_name->setObjectName(QString::fromUtf8("label_name"));

        horizontalLayout_2->addWidget(label_name);

        pushButton_3 = new QPushButton(dockWidgetContents_5);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout_2->addWidget(pushButton_3);


        verticalLayout_2->addLayout(horizontalLayout_2);

        labelWidget->setWidget(dockWidgetContents_5);
        label->addDockWidget(static_cast<Qt::DockWidgetArea>(2), labelWidget);
        fileWidget = new QDockWidget(label);
        fileWidget->setObjectName(QString::fromUtf8("fileWidget"));
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
        verticalLayout_4 = new QVBoxLayout(dockWidgetContents_2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        file_list = new CustomListWidget(dockWidgetContents_2);
        file_list->setObjectName(QString::fromUtf8("file_list"));

        verticalLayout_4->addWidget(file_list);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        not_labeld = new QRadioButton(dockWidgetContents_2);
        not_labeld->setObjectName(QString::fromUtf8("not_labeld"));
        not_labeld->setChecked(true);

        horizontalLayout_3->addWidget(not_labeld);

        labeld = new QRadioButton(dockWidgetContents_2);
        labeld->setObjectName(QString::fromUtf8("labeld"));
        labeld->setChecked(false);

        horizontalLayout_3->addWidget(labeld);


        verticalLayout_4->addLayout(horizontalLayout_3);

        fileWidget->setWidget(dockWidgetContents_2);
        label->addDockWidget(static_cast<Qt::DockWidgetArea>(2), fileWidget);

        toolBar->addAction(actionUndo);
        toolBar->addAction(actionRedo);
        toolBar->addAction(actionZoom_in);
        toolBar->addAction(actionZoom_out);
        toolBar->addAction(actionPrevious_Image);
        toolBar->addAction(actionNext_Image);
        menuBar->addAction(menu->menuAction());

        retranslateUi(label);

        QMetaObject::connectSlotsByName(label);
    } // setupUi

    void retranslateUi(QMainWindow *label)
    {
        label->setWindowTitle(QApplication::translate("label", "label", nullptr));
        actionUndo->setText(QApplication::translate("label", "Undo", nullptr));
#ifndef QT_NO_TOOLTIP
        actionUndo->setToolTip(QApplication::translate("label", "Undo", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionUndo->setShortcut(QApplication::translate("label", "Ctrl+Z", nullptr));
#endif // QT_NO_SHORTCUT
        actionRedo->setText(QApplication::translate("label", "Redo", nullptr));
#ifndef QT_NO_SHORTCUT
        actionRedo->setShortcut(QApplication::translate("label", "Ctrl+Shift+Z", nullptr));
#endif // QT_NO_SHORTCUT
        actionZoom_in->setText(QApplication::translate("label", "Zoom in", nullptr));
#ifndef QT_NO_SHORTCUT
        actionZoom_in->setShortcut(QApplication::translate("label", "Y", nullptr));
#endif // QT_NO_SHORTCUT
        actionZoom_out->setText(QApplication::translate("label", "Zoom out", nullptr));
#ifndef QT_NO_SHORTCUT
        actionZoom_out->setShortcut(QApplication::translate("label", "H", nullptr));
#endif // QT_NO_SHORTCUT
        actionPrevious_Image->setText(QApplication::translate("label", "Previous Image", nullptr));
#ifndef QT_NO_SHORTCUT
        actionPrevious_Image->setShortcut(QApplication::translate("label", "Left", nullptr));
#endif // QT_NO_SHORTCUT
        actionNext_Image->setText(QApplication::translate("label", "Next Image", nullptr));
#ifndef QT_NO_SHORTCUT
        actionNext_Image->setShortcut(QApplication::translate("label", "Right", nullptr));
#endif // QT_NO_SHORTCUT
        image->setText(QString());
        toolBar->setWindowTitle(QApplication::translate("label", "toolBar", nullptr));
        menu->setTitle(QApplication::translate("label", "\347\274\226\350\276\221", nullptr));
        annotationWidget->setWindowTitle(QApplication::translate("label", "\346\240\207\346\263\250\344\277\241\346\201\257", nullptr));
        pushButton->setText(QApplication::translate("label", "\344\270\212\347\247\273", nullptr));
        pushButton_2->setText(QApplication::translate("label", "\344\270\213\347\247\273", nullptr));
        labelWidget->setWindowTitle(QApplication::translate("label", "\347\261\273\345\210\253\344\277\241\346\201\257", nullptr));
        pushButton_3->setText(QApplication::translate("label", "\346\267\273\345\212\240\346\226\260\346\240\207\347\255\276", nullptr));
        fileWidget->setWindowTitle(QApplication::translate("label", "\346\226\207\344\273\266\345\210\227\350\241\250", nullptr));
        not_labeld->setText(QApplication::translate("label", "\346\234\252\346\240\207\346\263\250\346\226\207\344\273\266", nullptr));
        labeld->setText(QApplication::translate("label", "\345\267\262\346\240\207\346\263\250\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class label: public Ui_label {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LABEL_H
