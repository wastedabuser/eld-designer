/********************************************************************************
** Form generated from reading UI file 'editor.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITOR_H
#define UI_EDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Editor
{
public:
    QHBoxLayout *horizontalLayout_4;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *nodeType;
    QToolButton *addRootNode;
    QToolButton *addNode;
    QToolButton *removeNode;
    QSpacerItem *horizontalSpacer;
    QTreeView *treeView;
    QTableView *tableView;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;

    void setupUi(QWidget *Editor)
    {
        if (Editor->objectName().isEmpty())
            Editor->setObjectName(QStringLiteral("Editor"));
        Editor->resize(820, 562);
        horizontalLayout_4 = new QHBoxLayout(Editor);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(Editor);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        nodeType = new QComboBox(frame);
        nodeType->setObjectName(QStringLiteral("nodeType"));

        horizontalLayout_2->addWidget(nodeType);

        addRootNode = new QToolButton(frame);
        addRootNode->setObjectName(QStringLiteral("addRootNode"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        addRootNode->setIcon(icon);

        horizontalLayout_2->addWidget(addRootNode);

        addNode = new QToolButton(frame);
        addNode->setObjectName(QStringLiteral("addNode"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/table_row_insert.png"), QSize(), QIcon::Normal, QIcon::Off);
        addNode->setIcon(icon1);

        horizontalLayout_2->addWidget(addNode);

        removeNode = new QToolButton(frame);
        removeNode->setObjectName(QStringLiteral("removeNode"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/table_row_delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        removeNode->setIcon(icon2);

        horizontalLayout_2->addWidget(removeNode);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_2);

        treeView = new QTreeView(frame);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->header()->setVisible(false);

        verticalLayout_2->addWidget(treeView);

        tableView = new QTableView(frame);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->horizontalHeader()->setVisible(false);
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->verticalHeader()->setVisible(false);
        tableView->verticalHeader()->setDefaultSectionSize(20);

        verticalLayout_2->addWidget(tableView);

        verticalLayout_2->setStretch(1, 2);
        verticalLayout_2->setStretch(2, 1);

        horizontalLayout_4->addWidget(frame);

        scrollArea = new QScrollArea(Editor);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 613, 560));
        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout_4->addWidget(scrollArea);

        horizontalLayout_4->setStretch(0, 1);
        horizontalLayout_4->setStretch(1, 3);

        retranslateUi(Editor);

        QMetaObject::connectSlotsByName(Editor);
    } // setupUi

    void retranslateUi(QWidget *Editor)
    {
        Editor->setWindowTitle(QApplication::translate("Editor", "Form", 0));
        addRootNode->setText(QApplication::translate("Editor", "...", 0));
        addNode->setText(QApplication::translate("Editor", "...", 0));
        removeNode->setText(QApplication::translate("Editor", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class Editor: public Ui_Editor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITOR_H
