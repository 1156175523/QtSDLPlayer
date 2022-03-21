/********************************************************************************
** Form generated from reading UI file 'QtSDLPlay.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSDLPLAY_H
#define UI_QTSDLPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtSDLPlayClass
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *buttonLayout;
    QLineEdit *filePath;
    QPushButton *openFile;
    QPushButton *selectFile;
    QOpenGLWidget *videoWidget;

    void setupUi(QWidget *QtSDLPlayClass)
    {
        if (QtSDLPlayClass->objectName().isEmpty())
            QtSDLPlayClass->setObjectName(QStringLiteral("QtSDLPlayClass"));
        QtSDLPlayClass->resize(720, 628);
        horizontalLayoutWidget = new QWidget(QtSDLPlayClass);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(50, 30, 621, 61));
        buttonLayout = new QHBoxLayout(horizontalLayoutWidget);
        buttonLayout->setSpacing(6);
        buttonLayout->setContentsMargins(11, 11, 11, 11);
        buttonLayout->setObjectName(QStringLiteral("buttonLayout"));
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        filePath = new QLineEdit(horizontalLayoutWidget);
        filePath->setObjectName(QStringLiteral("filePath"));

        buttonLayout->addWidget(filePath);

        openFile = new QPushButton(horizontalLayoutWidget);
        openFile->setObjectName(QStringLiteral("openFile"));

        buttonLayout->addWidget(openFile);

        selectFile = new QPushButton(horizontalLayoutWidget);
        selectFile->setObjectName(QStringLiteral("selectFile"));

        buttonLayout->addWidget(selectFile);

        videoWidget = new QOpenGLWidget(QtSDLPlayClass);
        videoWidget->setObjectName(QStringLiteral("videoWidget"));
        videoWidget->setGeometry(QRect(160, 180, 331, 261));

        retranslateUi(QtSDLPlayClass);
        QObject::connect(selectFile, SIGNAL(clicked()), QtSDLPlayClass, SLOT(SelectLocalFile()));
        QObject::connect(openFile, SIGNAL(clicked()), QtSDLPlayClass, SLOT(OpenFile()));

        QMetaObject::connectSlotsByName(QtSDLPlayClass);
    } // setupUi

    void retranslateUi(QWidget *QtSDLPlayClass)
    {
        QtSDLPlayClass->setWindowTitle(QApplication::translate("QtSDLPlayClass", "QtSDLPlay", Q_NULLPTR));
        openFile->setText(QApplication::translate("QtSDLPlayClass", "\346\211\223\345\274\200", Q_NULLPTR));
        selectFile->setText(QApplication::translate("QtSDLPlayClass", "\351\200\211\346\213\251\346\226\207\344\273\266", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtSDLPlayClass: public Ui_QtSDLPlayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSDLPLAY_H
