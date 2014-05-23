#include "editor.h"
#include "jsonio.h"
#include "propertymodel.h"
#include "gameobjectmodel.h"
#include "ui_editor.h"
#include "config.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <qfiledialog.h>

Editor::Editor(QWidget *parent) : QWidget(parent),
	ui(new Ui::Editor) {
    ui->setupUi(this);

	gameObjectModel = new GameObjectModel();
	ui->treeView->setModel(gameObjectModel);
	ui->nodeType->addItems(Config::typesList);
}

Editor::~Editor() {
    delete ui;
}

void Editor::load(const QString &fileNm) {
	fileName = fileNm;
	if (fileName.isEmpty()) return;

	QJsonDocument doc = JsonIO::readJson(fileName);
	gameObjectModel->setJson(doc);
}

void Editor::save() {
	if (fileName.isEmpty())
        fileName = QFileDialog::getSaveFileName(this, tr("Open File"), "", tr("Files (*.txt;*.json;*.lvjson;*.aijson)"));

	if (!fileName.isEmpty()) {
        JsonIO::writeJson(fileName, gameObjectModel->getJson());
		QTabWidget *tabs = (QTabWidget*) parentWidget()->parentWidget();
		QFileInfo info1(fileName);
		tabs->setTabText(tabIndex, info1.fileName());
	}
}

void Editor::on_addNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();

    QString typeName = ui->nodeType->currentText();
	gameObjectModel->createGameObject(typeName, index);
}

void Editor::on_removeNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	gameObjectModel->removeGameObject(index);
	view->selectionModel()->clearCurrentIndex();
	view->clearSelection();
}

void Editor::on_treeView_clicked(const QModelIndex &index) {
    ui->tableView->setModel(gameObjectModel->getItem(index)->propertyModel);
}

void Editor::on_addRootNode_clicked() {
	QString typeName = ui->nodeType->currentText();
	gameObjectModel->createGameObject(typeName, QModelIndex());
}
