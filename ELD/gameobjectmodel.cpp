#include <QtGui>

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
	doc.setArray(rootItem->getChildJsonArray());
	return doc;
}

void GameObjectModel::setJson(const QJsonDocument &doc) {
	QJsonArray list = doc.array();
	if (list.isEmpty()) Util::warn("Document is empty");
	beginInsertRows(QModelIndex(), 0, list.size() - 1);
	rootItem->createChildrenFromJsonArray(list);
	endInsertRows();
}

void GameObjectModel::createGameObject(const QString &typeName, const QModelIndex &index) {
	GameObject *item = getItem(index);

	beginInsertRows(index, item->childCount(), item->childCount());
	QJsonObject obj;
	obj["type"] = typeName;
	obj["id"] = getNextGameObjectId(typeName);
	item->appendChild(obj);
	endInsertRows();
}

void GameObjectModel::removeGameObject(const QModelIndex &index) {
	GameObject *item = getItem(index);

	beginRemoveRows(index, index.row(), index.row());
	item->parent()->removeChild(index.row());
	endRemoveRows();
}
