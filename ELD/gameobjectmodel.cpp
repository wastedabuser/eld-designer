#include <QMessageBox>
#include <QtGui>

#include "config.h"
#include "gameobject.h"
#include "gameobjectmodel.h"
#include "util.h"

GameObjectModel::GameObjectModel(QObject *parent) : QAbstractItemModel(parent) {
	itemIdCount = 1;
	rootItem = new GameObject(QJsonObject());
}

GameObjectModel::~GameObjectModel() {
    delete rootItem;
}

int GameObjectModel::rowCount(const QModelIndex &parent) const {
	GameObject *parentItem = getItem(parent);

	return parentItem->childCount();
}

int GameObjectModel::columnCount(const QModelIndex & /* parent */) const {
	return 1;
}

QModelIndex GameObjectModel::index(int row, int column, const QModelIndex &parent) const {
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	GameObject *parentItem = getItem(parent);
	GameObject *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex GameObjectModel::parent(const QModelIndex &index) const {
	if (!index.isValid())
		return QModelIndex();

	GameObject *childItem = getItem(index);
	GameObject *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

QVariant GameObjectModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    GameObject *item = getItem(index);
	return item->id;
}

bool GameObjectModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (role != Qt::EditRole)
		return false;

	GameObject *item = getItem(index);
	item->id = value.toString();
	emit dataChanged(index, index);

	return true;
}

Qt::ItemFlags GameObjectModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex GameObjectModel::getGameObjectIndex(GameObject *obj) const {
	return createIndex(obj->childNumber(), 0, obj);
}

GameObject *GameObjectModel::getItem(const QModelIndex &index) const {
	if (index.isValid()) {
		GameObject *item = static_cast<GameObject*>(index.internalPointer());
		if (item) return item;
	}
	return rootItem;
}

QString GameObjectModel::getNextGameObjectId(const QString &typeName) {
	QString id = "-";
	id.prepend(typeName);
	id.append(QString::number(itemIdCount));
	itemIdCount++;
	return id;
}

QJsonDocument GameObjectModel::getJson() {
	QJsonDocument doc;
	doc.setObject(rootItem->getJsonObject());
	return doc;
}

QList<GameObject *> GameObjectModel::setJson(const QJsonDocument &doc) {
	QJsonObject obj = doc.object();
	if (obj.isEmpty()) Util::warn("Document is empty");
	rootItem->type = obj["type"].toString();

	QJsonArray list = obj["children"].toArray();
	if (list.isEmpty()) return QList<GameObject *>();
	beginInsertRows(QModelIndex(), 0, 0);
	rootItem->createChildrenFromJsonArray(list);
	endInsertRows();
	return rootItem->getChildren();
}

bool GameObjectModel::canCreateObject(const QString &typeName, const QModelIndex &index) {
	GameObject *item = getItem(index);
	QJsonArray allowed = Config::alowedChildObject[item->type];
	bool ok;
	for (int i = 0; i < allowed.size(); i++) {
		if (allowed[i].toString() == typeName) {
			ok = true;
			break;
		}
	}
	return ok;
}

GameObject *GameObjectModel::appendGameObjectFromJsonObject(const QJsonObject &obj, const QModelIndex &index) {
	QString typeName = obj["type"].toString();
	if (!canCreateObject(typeName, index)) {
		QMessageBox messageBox;
		messageBox.warning(0, tr("Not allowed"), tr("You can not create such object under the current selection"));
		return 0;
	}

	GameObject *item = getItem(index);
	beginInsertRows(index, item->childCount(), item->childCount());
	GameObject *child = item->appendChild(obj);
	endInsertRows();

	return child;
}

GameObject *GameObjectModel::createGameObject(const QString &typeName, const QModelIndex &index) {
	QJsonObject obj;
	obj["type"] = typeName;
	obj["id"] = getNextGameObjectId(typeName);
	return appendGameObjectFromJsonObject(obj, index);
}

GameObject *GameObjectModel::removeGameObject(const QModelIndex &index) {
	GameObject *item = getItem(index);

	beginRemoveRows(parent(index), item->childNumber(), item->childNumber());
	item->parent()->removeChild(index.row());
	endRemoveRows();

	return item;
}

bool GameObjectModel::moveGameObject(const QModelIndex &pindex, int offset) {
	int of = offset + (offset > 0 ? 1: 0);
	if (pindex.row() + of < 0) return false;
	if (!beginMoveRows(pindex, pindex.row(), pindex.row(), pindex, pindex.row() + of))
		return false;

	GameObject *item = getItem(pindex);
	GameObject *parent = item->parent();
	parent->removeChild(pindex.row());
	parent->addChild(pindex.row() + offset, item);
	endMoveRows();

	return true;
}
