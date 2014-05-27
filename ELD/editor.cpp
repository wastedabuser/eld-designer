#include "editor.h"
#include "jsonio.h"
#include "propertymodel.h"
#include "gameobjectmodel.h"
#include "ui_editor.h"
#include "config.h"
#include "gameobjectcontainer.h"

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

	gameObjectContainer = new GameObjectContainer();
	ui->scrollArea->setWidget(gameObjectContainer);
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

void Editor::addNode(const QModelIndex &index) {
	QString typeName = ui->nodeType->currentText();
	GameObject *item = gameObjectModel->createGameObject(typeName, index);
	if (item->hasView())
		gameObjectContainer->addGameObject(item);
}

void Editor::on_addNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();
	addNode(index);
}

void Editor::on_removeNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	GameObject *item = gameObjectModel->removeGameObject(index);
	if (item->hasView())
		gameObjectContainer->removeGameObject(item);

	view->selectionModel()->clearCurrentIndex();
	view->clearSelection();
}

void Editor::on_treeView_clicked(const QModelIndex &index) {
    ui->tableView->setModel(gameObjectModel->getItem(index)->propertyModel);
}

void Editor::on_addRootNode_clicked() {
	addNode(QModelIndex());
}
