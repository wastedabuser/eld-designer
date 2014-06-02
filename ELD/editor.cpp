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

	connect(gameObjectContainer, SIGNAL(gameObjectSelect(GameObject *)),
			this, SLOT(on_gameObject_selected(GameObject *)));
}

Editor::~Editor() {
	disconnect(gameObjectContainer, SIGNAL(gameObjectSelect(GameObject *)),
			this, SLOT(on_gameObject_selected(GameObject *)));

    delete ui;
}

void Editor::load(const QString &fileNm) {
	fileName = fileNm;
	if (fileName.isEmpty()) return;

	QJsonDocument doc = JsonIO::readJson(fileName);
	QList<GameObject *> list = gameObjectModel->setJson(doc);
	for (int i = list.size() - 1; i >= 0; i--) {
		gameObjectContainer->addGameObject(list[i], false);
	}
	gameObjectContainer->updateCanvas();
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
	gameObjectContainer->addGameObject(item);

	applyGameObjectsOrder();
}

GameObject *Editor::copyGameObject() {
	QTreeView *view = ui->treeView;
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return 0;

	GameObject *item = gameObjectModel->getItem(index);
	return item->cloneDeep();
}

GameObject *Editor::cutGameObject() {
	QTreeView *view = ui->treeView;
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return 0;

	GameObject *item = gameObjectModel->removeGameObject(index);
	gameObjectContainer->removeGameObject(item);

	applyGameObjectsOrder();

	return item;
}

void Editor::pasteGameObject(GameObject *obj) {
	QTreeView *view = ui->treeView;
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	GameObject *item = gameObjectModel->appendGameObjectFromJsonObject(obj->getJsonObject(), index);
	gameObjectContainer->addGameObject(item);

	applyGameObjectsOrder();
}

void Editor::applyGameObjectsOrder() {
	QList<GameObject *> list = gameObjectModel->rootItem->getChildrenListDeep();
	gameObjectContainer->applyGameObjectsOrder(list);
}

void Editor::on_addNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();
	addNode(index);
}

void Editor::on_gameObject_selected(GameObject *obj) {
	QModelIndex newIndex = gameObjectModel->getGameObjectIndex(obj);
	ui->treeView->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::ClearAndSelect);
	ui->tableView->setModel(obj->propertyModel);
}

void Editor::on_removeNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	GameObject *item = gameObjectModel->removeGameObject(index);
	gameObjectContainer->removeGameObject(item);

	view->selectionModel()->clearCurrentIndex();
	view->clearSelection();

	delete item;
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

	applyGameObjectsOrder();
	QModelIndex newIndex = gameObjectModel->index(index.row() - 1, 0, index.parent());
	view->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::ClearAndSelect);
}

void Editor::on_moveNodeDown_clicked() {
	QTreeView *view = ui->treeView;
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	if (!gameObjectModel->moveGameObject(index, 1))
		return;

	applyGameObjectsOrder();
	QModelIndex newIndex = gameObjectModel->index(index.row() + 1, 0, index.parent());
	if (newIndex.isValid())
		view->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::ClearAndSelect);
}

void Editor::on_expandTree_clicked() {
	QTreeView *view = ui->treeView;
	view->expandAll();
}
