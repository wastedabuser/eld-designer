#include <QMessageBox>
#include <QtGui>

#include "config.h"
#include "gameobject.h"
#include "gameobjectmodel.h"
#include "util.h"

GameObjectModel::GameObjectModel(QObject *parent) : QAbstractItemModel(parent) {
	itemIdCount = 1;
	rootItem = 0;
}

GameObjectModel::~GameObjectModel() {
	if (rootItem) delete rootItem;
}

int GameObjectModel::rowCount(const QModelIndex &parent) const {
	if (!rootItem) return 0;
	GameObject *parentItem = getItem(parent);
	return parentItem->childCount();
}

int GameObjectModel::columnCount(const QModelIndex & /* parent */) const {
	return 1;
}

QModelIndex GameObjectModel::index(int row, int column, const QModelIndex &parent) const {
	if (!rootItem) return QModelIndex();

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
	emit gameObjectChanged();

	return true;
}

Qt::ItemFlags GameObjectModel::flags(const QModelIndex &index) const {
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

//	| Qt::ItemIsDragEnabled

	if (index.isValid())
		return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | defaultFlags;
	else
		return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList GameObjectModel::mimeTypes() const {
	QStringList types;
	types << "application/eld-designer.object-type";
	return types;
}

bool GameObjectModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/eld-designer.object-type"))
		return false;

	QByteArray encodedData = data->data("application/eld-designer.object-type");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QStringList newItems;

	while (!stream.atEnd()) {
		QString text;
		stream >> text;
		newItems << text;
	}

	foreach (QString text, newItems) {
		createGameObject(text, parent);
	}

	return true;
}

QModelIndex GameObjectModel::getGameObjectIndex(GameObject *obj) const {
	if (obj == rootItem) return QModelIndex();
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

void GameObjectModel::setJson(const QJsonDocument &doc) {
	QJsonObject obj = doc.object();
	if (obj.isEmpty()) Util::warn("Document is empty");

	setJsonObject(obj);
}

void GameObjectModel::setJsonObject(const QJsonObject &obj) {
	beginInsertRows(QModelIndex(), 0, 0);
	rootItem = new GameObject(this, obj);
	endInsertRows();
}

bool GameObjectModel::canCreateObject(const QString &typeName, const QModelIndex &index) {
	GameObject *item = getItem(index);
	QJsonArray allowed = Config::alowedChildObject[item->type];
	QJsonObject typeDef = Config::typesDefinitions[typeName];
	QString categoryType = typeDef["category"].toString();
	bool ok;
	for (int i = 0; i < allowed.size(); i++) {
		QString ch = allowed[i].toString();
		if (ch == typeName || (!categoryType.isEmpty() && ch.right(1) == "*" && ch.left(ch.length() - 2) == categoryType)) {
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
	GameObject *child = item->appendChild(obj, this);
	endInsertRows();

	emit gameObjectAdded(child);
	emit gameObjectChanged();

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

	emit gameObjectChanged();

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

	emit gameObjectChanged();

	return true;
}

void GameObjectModel::onPropertyModelChanged(GameObject *obj) {
	emit gameObjectChanged();
}
