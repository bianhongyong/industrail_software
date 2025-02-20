/********************************************************************************
** Form generated from reading UI file 'collect.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLLECT_H
#define UI_COLLECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <widgets/customvideo.h>
#include <widgets/extendedcombobox.h>

QT_BEGIN_NAMESPACE

class Ui_collect
{
public:
    QAction *action_shot;
    QAction *action_collect;
    QAction *action_settings;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QToolBox *toolBox;
    QWidget *page1;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QLabel *camera;
    QComboBox *comboBox;
    QLabel *image_re;
    ExtendedComboBox *size;
    QLabel *video_re;
    QComboBox *image_re1;
    QWidget *page2;
    QVBoxLayout *verticalLayout_3;
    QFormLayout *formLayout_2;
    QLabel *label;
    QComboBox *comboBox_2;
    QPushButton *pushButton;
    QWidget *page3;
    QSpacerItem *verticalSpacer;
    CustomVideo *video;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *collect)
    {
        if (collect->objectName().isEmpty())
            collect->setObjectName(QString::fromUtf8("collect"));
        collect->resize(1001, 500);
        collect->setStyleSheet(QString::fromUtf8(""));
        action_shot = new QAction(collect);
        action_shot->setObjectName(QString::fromUtf8("action_shot"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/icons/shot.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_shot->setIcon(icon);
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(28);
        action_shot->setFont(font);
        action_collect = new QAction(collect);
        action_collect->setObjectName(QString::fromUtf8("action_collect"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/collect.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_collect->setIcon(icon1);
        action_settings = new QAction(collect);
        action_settings->setObjectName(QString::fromUtf8("action_settings"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_settings->setIcon(icon2);
        centralwidget = new QWidget(collect);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        toolBox = new QToolBox(centralwidget);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolBox->sizePolicy().hasHeightForWidth());
        toolBox->setSizePolicy(sizePolicy);
        toolBox->setMinimumSize(QSize(400, 0));
        toolBox->setMaximumSize(QSize(16777215, 16777215));
        QFont font1;
        font1.setPointSize(18);
        toolBox->setFont(font1);
        page1 = new QWidget();
        page1->setObjectName(QString::fromUtf8("page1"));
        page1->setGeometry(QRect(0, 0, 609, 123));
        verticalLayout_2 = new QVBoxLayout(page1);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        camera = new QLabel(page1);
        camera->setObjectName(QString::fromUtf8("camera"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(18);
        camera->setFont(font2);

        formLayout->setWidget(0, QFormLayout::LabelRole, camera);

        comboBox = new QComboBox(page1);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setEnabled(true);
        sizePolicy.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
        comboBox->setSizePolicy(sizePolicy);
        comboBox->setMinimumSize(QSize(0, 0));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(16);
        comboBox->setFont(font3);
        comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

        formLayout->setWidget(0, QFormLayout::FieldRole, comboBox);

        image_re = new QLabel(page1);
        image_re->setObjectName(QString::fromUtf8("image_re"));
        image_re->setFont(font2);

        formLayout->setWidget(2, QFormLayout::LabelRole, image_re);

        size = new ExtendedComboBox(page1);
        size->addItem(QString());
        size->addItem(QString());
        size->addItem(QString());
        size->addItem(QString());
        size->addItem(QString());
        size->addItem(QString());
        size->addItem(QString());
        size->addItem(QString());
        size->setObjectName(QString::fromUtf8("size"));
        sizePolicy.setHeightForWidth(size->sizePolicy().hasHeightForWidth());
        size->setSizePolicy(sizePolicy);

        formLayout->setWidget(2, QFormLayout::FieldRole, size);

        video_re = new QLabel(page1);
        video_re->setObjectName(QString::fromUtf8("video_re"));
        video_re->setFont(font1);

        formLayout->setWidget(1, QFormLayout::LabelRole, video_re);

        image_re1 = new QComboBox(page1);
        image_re1->addItem(QString());
        image_re1->setObjectName(QString::fromUtf8("image_re1"));
        image_re1->setLayoutDirection(Qt::LeftToRight);
        image_re1->setEditable(false);

        formLayout->setWidget(1, QFormLayout::FieldRole, image_re1);


        verticalLayout_2->addLayout(formLayout);

        toolBox->addItem(page1, QString::fromUtf8("\345\233\276\345\203\217\346\272\220"));
        page2 = new QWidget();
        page2->setObjectName(QString::fromUtf8("page2"));
        page2->setGeometry(QRect(0, 0, 623, 102));
        verticalLayout_3 = new QVBoxLayout(page2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label = new QLabel(page2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        comboBox_2 = new QComboBox(page2);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboBox_2);

        pushButton = new QPushButton(page2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        formLayout_2->setWidget(1, QFormLayout::SpanningRole, pushButton);


        verticalLayout_3->addLayout(formLayout_2);

        toolBox->addItem(page2, QString::fromUtf8("\346\250\241\345\205\267\345\217\202\346\225\260\351\200\211\346\213\251\344\270\216\345\275\225\345\205\245"));
        page3 = new QWidget();
        page3->setObjectName(QString::fromUtf8("page3"));
        page3->setGeometry(QRect(0, 0, 623, 102));
        toolBox->addItem(page3, QString::fromUtf8("\345\205\266\344\273\226"));

        verticalLayout->addWidget(toolBox);

        verticalSpacer = new QSpacerItem(78, 88, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        video = new CustomVideo(centralwidget);
        video->setObjectName(QString::fromUtf8("video"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(video->sizePolicy().hasHeightForWidth());
        video->setSizePolicy(sizePolicy1);
        video->setMinimumSize(QSize(300, 300));
        video->setMaximumSize(QSize(16777215, 16777215));
        video->setBaseSize(QSize(0, 0));

        horizontalLayout->addWidget(video);

        collect->setCentralWidget(centralwidget);
        menubar = new QMenuBar(collect);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1001, 21));
        collect->setMenuBar(menubar);
        statusbar = new QStatusBar(collect);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        collect->setStatusBar(statusbar);
        toolBar = new QToolBar(collect);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(toolBar->sizePolicy().hasHeightForWidth());
        toolBar->setSizePolicy(sizePolicy2);
        toolBar->setLayoutDirection(Qt::RightToLeft);
        toolBar->setMovable(true);
        toolBar->setAllowedAreas(Qt::AllToolBarAreas);
        toolBar->setOrientation(Qt::Vertical);
        toolBar->setIconSize(QSize(36, 36));
        toolBar->setFloatable(false);
        collect->addToolBar(Qt::LeftToolBarArea, toolBar);

        toolBar->addSeparator();
        toolBar->addAction(action_shot);
        toolBar->addAction(action_collect);
        toolBar->addAction(action_settings);

        retranslateUi(collect);

        toolBox->setCurrentIndex(0);
        toolBox->layout()->setSpacing(6);


        QMetaObject::connectSlotsByName(collect);
    } // setupUi

    void retranslateUi(QMainWindow *collect)
    {
        collect->setWindowTitle(QApplication::translate("collect", "MainWindow", nullptr));
        action_shot->setText(QApplication::translate("collect", "\346\213\215\347\205\247", nullptr));
#ifndef QT_NO_TOOLTIP
        action_shot->setToolTip(QApplication::translate("collect", "\346\213\215\347\205\247\344\277\235\345\255\230", nullptr));
#endif // QT_NO_TOOLTIP
        action_collect->setText(QApplication::translate("collect", "\350\207\252\345\212\250\351\207\207\351\233\206", nullptr));
        action_settings->setText(QApplication::translate("collect", "\350\256\276\347\275\256", nullptr));
        camera->setText(QApplication::translate("collect", "\347\233\270\346\234\272:", nullptr));
        image_re->setText(QApplication::translate("collect", "\345\233\276\347\211\207\345\210\206\350\276\250\347\216\207:", nullptr));
        size->setItemText(0, QApplication::translate("collect", "640\303\227480", nullptr));
        size->setItemText(1, QApplication::translate("collect", "800\303\227600", nullptr));
        size->setItemText(2, QApplication::translate("collect", "1024\303\227768", nullptr));
        size->setItemText(3, QApplication::translate("collect", "1280\303\227720", nullptr));
        size->setItemText(4, QApplication::translate("collect", "1280\303\227768", nullptr));
        size->setItemText(5, QApplication::translate("collect", "1920\303\2271080", nullptr));
        size->setItemText(6, QApplication::translate("collect", "2560\303\2271440", nullptr));
        size->setItemText(7, QApplication::translate("collect", "3840\303\2272160", nullptr));

        video_re->setText(QApplication::translate("collect", "\350\247\206\351\242\221\345\210\206\350\276\250\347\216\207", nullptr));
        image_re1->setItemText(0, QApplication::translate("collect", "1280\303\227720", nullptr));

        toolBox->setItemText(toolBox->indexOf(page1), QApplication::translate("collect", "\345\233\276\345\203\217\346\272\220", nullptr));
        label->setText(QApplication::translate("collect", "\346\250\241\345\205\267\351\200\211\346\213\251:", nullptr));
        pushButton->setText(QApplication::translate("collect", "\346\250\241\345\205\267\345\217\202\346\225\260\347\256\241\347\220\206", nullptr));
        toolBox->setItemText(toolBox->indexOf(page2), QApplication::translate("collect", "\346\250\241\345\205\267\345\217\202\346\225\260\351\200\211\346\213\251\344\270\216\345\275\225\345\205\245", nullptr));
        toolBox->setItemText(toolBox->indexOf(page3), QApplication::translate("collect", "\345\205\266\344\273\226", nullptr));
        toolBar->setWindowTitle(QApplication::translate("collect", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class collect: public Ui_collect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLLECT_H
