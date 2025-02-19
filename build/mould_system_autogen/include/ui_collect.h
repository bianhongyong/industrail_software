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
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <widgets/customvideo.h>

QT_BEGIN_NAMESPACE

class Ui_collect
{
public:
    QAction *action_usb;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    CustomVideo *video;
    QHBoxLayout *horizontalLayout_2;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *shot;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *collect)
    {
        if (collect->objectName().isEmpty())
            collect->setObjectName(QString::fromUtf8("collect"));
        collect->resize(800, 600);
        action_usb = new QAction(collect);
        action_usb->setObjectName(QString::fromUtf8("action_usb"));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(28);
        action_usb->setFont(font);
        centralwidget = new QWidget(collect);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        video = new CustomVideo(centralwidget);
        video->setObjectName(QString::fromUtf8("video"));

        verticalLayout->addWidget(video);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(frame_2);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(26);
        label->setFont(font1);

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(frame_2);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
        comboBox->setSizePolicy(sizePolicy);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(16);
        comboBox->setFont(font2);

        horizontalLayout->addWidget(comboBox);


        horizontalLayout_2->addWidget(frame_2);

        shot = new QPushButton(centralwidget);
        shot->setObjectName(QString::fromUtf8("shot"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(24);
        shot->setFont(font3);

        horizontalLayout_2->addWidget(shot);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Arial"));
        font4.setPointSize(22);
        pushButton->setFont(font4);

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_2);

        collect->setCentralWidget(centralwidget);
        menubar = new QMenuBar(collect);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        collect->setMenuBar(menubar);
        statusbar = new QStatusBar(collect);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        collect->setStatusBar(statusbar);

        retranslateUi(collect);

        QMetaObject::connectSlotsByName(collect);
    } // setupUi

    void retranslateUi(QMainWindow *collect)
    {
        collect->setWindowTitle(QApplication::translate("collect", "MainWindow", nullptr));
        action_usb->setText(QApplication::translate("collect", "\346\220\234\347\264\242usb\345\217\243", nullptr));
#ifndef QT_NO_TOOLTIP
        action_usb->setToolTip(QApplication::translate("collect", "<html><head/><body><p><span style=\" font-weight:600;\">\346\220\234\347\264\242usb\345\217\243</span></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("collect", "\347\233\270\346\234\272:", nullptr));
        shot->setText(QApplication::translate("collect", "\346\213\215\347\205\247", nullptr));
        pushButton->setText(QApplication::translate("collect", "\350\207\252\345\212\250\351\207\207\351\233\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class collect: public Ui_collect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLLECT_H
