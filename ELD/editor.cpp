#include "editor.h"
#include "jsonio.h"
#include "propertymodel.h"
#include "gameobjectmodel.h"
#include "ui_editor.h"
#include "config.h"
#include "gameobjectcontainer.h"
#include "propertyeditordelegate.h"
#include "filepropertydelegate.h"
#include "mainwindow.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QScrollBar>

Editor::Editor(MainWindow *mainW, QWidget *parent) : QWidget(parent),
	ui(new Ui::Editor) {
    ui->setupUi(this);

	mainWindow = mainW;

	gameObjectModel = new GameObjectModel();
	ui->treeView->setModel(gameObjectModel);

	ui->tableView->setItemDelegate(new PropertyEditorDelegate());

	gameObjectContainer = new GameObjectContainer();
	ui->scrollArea->setWidget(gameObjectContainer);

	connect(gameObjectContainer, SIGNAL(gameObjectSelect(GameObject *)), this, SLOT(on_gameObject_selected(GameObject *)));

	connect(gameObjectModel, SIGNAL(gameObjectChanged()), this, SLOT(onGameObjectChanged()));
	connect(gameObjectModel, SIGNAL(gameObjectAdded(GameObject *)), this, SLOT(onGameObjectAdded(GameObject *)));
	connect(gameObjectModel, SIGNAL(gameObjectRemoved(GameObject *)), this, SLOT(onGameObjectRemoved(GameObject *)));
}

Editor::~Editor() {
	disconnect(gameObjectContainer, SIGNAL(gameObjectSelect(GameObject *)),
			   this, SLOT(on_gameObject_selected(GameObject *)));

    delete ui;
}

void Editor::createNew(QJsonObject &obj) {
	gameObjectModel->setJsonObject(obj);
	gameObjectContainer->addGameObject(gameObjectModel->rootItem);
}

void Editor::load(const QString &fileNm) {
	fileName = fileNm;
	if (fileName.isEmpty()) return;

	QJsonDocument doc = JsonIO::readJson(fileName);
    QJsonObject editorMeta = gameObjectModel->setJson(doc);
	gameObjectContainer->addGameObject(gameObjectModel->rootItem, false);

    if (!editorMeta.isEmpty()) {
        QList<int> sizes;
        if (editorMeta.contains("treeWidth")) {
            sizes.append(editorMeta["treeWidth"].toInt());
            sizes.append(editorMeta["workspaceWidth"].toInt());
            ui->splitter->setSizes(sizes);
            sizes.clear();
        }
        if (editorMeta.contains("treeHeight")) {
            sizes.append(editorMeta["treeHeight"].toInt());
            sizes.append(editorMeta["propertiesHeight"].toInt());
            ui->splitter_2->setSizes(sizes);
            sizes.clear();
        }
        if (editorMeta.contains("scaleFactor")) {
            gameObjectContainer->setScaleFactor(editorMeta["scaleFactor"].toDouble());
        } else {
            gameObjectContainer->updateCanvas();
        }
    } else {
        gameObjectContainer->updateCanvas();
    }

	QTimer::singleShot(0, this, SLOT(onEditorReady()));
}

void Editor::onEditorReady() {
	gameObjectContainer->centerView();
}

void Editor::save() {
    QJsonObject editorMeta;
    QList<int> sizes = ui->splitter->sizes();
    editorMeta["treeWidth"] = sizes[0];
    editorMeta["workspaceWidth"] = sizes[1];
    sizes = ui->splitter_2->sizes();
    editorMeta["treeHeight"] = sizes[0];
    editorMeta["propertiesHeight"] = sizes[1];

    editorMeta["scaleFactor"] = gameObjectContainer->getScaleFactor();

    QJsonDocument doc = gameObjectModel->getJson(editorMeta);
	QString newName = doc.object()["id"].toString();
	if (!newName.isEmpty()) newName = "/" + newName + ".json";

	if (fileName.isEmpty()) {
		fileName = QFileDialog::getSaveFileName(this, tr("Open File"), mainWindow->lastOpenDir() + newName, tr("Files (*.txt;*.json)"));
		if (!fileName.isEmpty()) {
			mainWindow->addRecentFile(fileName);
			QFileInfo info1(fileName);
			QString lastOpenDir = info1.absolutePath();
			mainWindow->applySetting("lastOpenDir", lastOpenDir);
		}
	}

	if (!fileName.isEmpty()) {
		JsonIO::writeJson(fileName, doc);                
		mainWindow->updateEditorIndexes();
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
	ObjectType *tp = mainWindow->getSelectedObjectType();
	if (!tp) return;

	QString typeName = tp->type;
	gameObjectModel->createGameObject(typeName, index);
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

	return gameObjectModel->removeGameObject(index);
}

void Editor::pasteGameObject(GameObject *obj) {
	QTreeView *view = ui->treeView;
	QModelIndex index = view->selectionModel()->currentIndex();

	gameObjectModel->createFromJsonObject(obj->getJsonObject(), index);
}

void Editor::applyGameObjectsOrder() {
	QList<GameObject *> list = gameObjectModel->rootItem->getChildrenListDeep();
	gameObjectContainer->applyGameObjectsOrder(list);
}

void Editor::onGameObjectChanged() {
	if (!parentWidget()) return;

	mainWindow->updateEditorIndexes();
	QTabWidget *tabs = (QTabWidget*) parentWidget()->parentWidget();
	QString label = tabs->tabText(tabIndex);
	if (label.right(3) == "[*]") return;
	else {
		tabs->setTabText(tabIndex, label + " [*]");
	}
}

void Editor::onGameObjectAdded(GameObject *obj) {
	gameObjectContainer->addGameObject(obj);
	applyGameObjectsOrder();
}

void Editor::onGameObjectRemoved(GameObject *obj) {
	gameObjectContainer->removeGameObject(obj);
	applyGameObjectsOrder();
}

void Editor::on_addNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();
	addNode(index);
}

void Editor::on_gameObject_selected(GameObject *obj) {
	QModelIndex newIndex = gameObjectModel->getGameObjectIndex(obj);
	if (newIndex.isValid())
		ui->treeView->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::ClearAndSelect);
	ui->tableView->setModel(obj->propertyModel);
}

void Editor::on_removeNode_clicked() {
    QTreeView *view = ui->treeView;
    QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	GameObject *item = gameObjectModel->removeGameObject(index);

	view->selectionModel()->clearCurrentIndex();
	view->clearSelection();

	delete item;
}

void Editor::on_treeView_clicked(const QModelIndex &index) {
	GameObject *item = gameObjectModel->getItem(index);
	ui->tableView->setModel(item->propertyModel);
	gameObjectContainer->selectGameObject(item);
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

void Editor::on_editRoot_clicked() {
	QTreeView *view = ui->treeView;
	view->selectionModel()->clearCurrentIndex();
	view->clearSelection();

	ui->tableView->setModel(gameObjectModel->rootItem->propertyModel);
}
