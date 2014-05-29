#include "editor.h"
#include "jsonio.h"
#include "propertymodel.h"
#include "gameobjectmodel.h"
#include "ui_editor.h"
#include "config.h"
#include "gameobjectcontainer.h"
#include "propertyeditordelegate.h"
#include "filepropertydelegate.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <qfiledialog.h>

Editor::Editor(QWidget *parent) : QWidget(parent),
	ui(new Ui::Editor) {
    ui->setupUi(this);

	ui->nodeType->addItems(Config::typesList);

	gameObjectModel = new GameObjectModel();
	ui->treeView->setModel(gameObjectModel);

	ui->tableView->setItemDelegate(new PropertyEditorDelegate());

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
	QList<GameObject *> list = gameObjectModel->setJson(doc);
	for (int i = 0; i < list.size(); i++) {
		GameObject *item = list[i];
		if (item->hasView())
			gameObjectContainer->addGameObject(item);
	}
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

void Editor::zoomIn() {
	gameObjectContainer->zoom(true);
}

void Editor::zoomOut() {
	gameObjectContainer->zoom(false);
}

void Editor::addNode(const QModelIndex &index) {
	QString typeName = ui->nodeType->currentText();
	GameObject *item = gameObjectModel->createGameObject(typeName, index);
	if (item && item->hasView())
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
	GameObject *item = gameObjectModel->getItem(index);
	ui->tableView->setModel(item->propertyModel);
	gameObjectContainer->selectGameObject(item);
}

void Editor::on_addRootNode_clicked() {
	addNode(QModelIndex());
}

void Editor::on_moveNodeUp_clicked() {
	QTreeView *view = ui->treeView;
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	if (!gameObjectModel->moveGameObject(index, -1))
		return;

	QModelIndex newIndex = gameObjectModel->index(index.row() - 1, 0, index.parent());
	view->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::ClearAndSelect);
}

void Editor::on_moveNodeDown_clicked() {
	QTreeView *view = ui->treeView;
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	if (!gameObjectModel->moveGameObject(index, 1))
		return;

	QModelIndex newIndex = gameObjectModel->index(index.row() + 1, 0, index.parent());
	if (newIndex.isValid())
		view->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::ClearAndSelect);
}
