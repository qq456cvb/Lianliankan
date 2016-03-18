/********************************************************************************
** Form generated from reading UI file 'lianliankan.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIANLIANKAN_H
#define UI_LIANLIANKAN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LianliankanClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *beginButton;
    QPushButton *pushButton_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *LianliankanClass)
    {
        if (LianliankanClass->objectName().isEmpty())
            LianliankanClass->setObjectName(QStringLiteral("LianliankanClass"));
        LianliankanClass->resize(600, 400);
        centralWidget = new QWidget(LianliankanClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        beginButton = new QPushButton(centralWidget);
        beginButton->setObjectName(QStringLiteral("beginButton"));

        verticalLayout->addWidget(beginButton);

        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        LianliankanClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(LianliankanClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        LianliankanClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(LianliankanClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        LianliankanClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(LianliankanClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        LianliankanClass->setStatusBar(statusBar);

        retranslateUi(LianliankanClass);

        QMetaObject::connectSlotsByName(LianliankanClass);
    } // setupUi

    void retranslateUi(QMainWindow *LianliankanClass)
    {
        LianliankanClass->setWindowTitle(QApplication::translate("LianliankanClass", "Lianliankan", 0));
        beginButton->setText(QApplication::translate("LianliankanClass", "test", 0));
        pushButton_2->setText(QApplication::translate("LianliankanClass", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class LianliankanClass: public Ui_LianliankanClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIANLIANKAN_H
