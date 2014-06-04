#include "objecttypemodel.h"

#include <QMimeData>;

ObjectTypeModel::ObjectTypeModel(QList<QJsonObject> &list, QObject *parent) :
	QAbstractListModel(parent) {

	setObjectTypes(list);
}

ObjectTypeModel::~ObjectTypeModel() {
	qDeleteAll(types);
}

int ObjectTypeModel::rowCount(const QModelIndex &parent) const {
	return types.count();
}

QVariant ObjectTypeModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	ObjectType *item = getItem(index);
	return item->type;
}

Qt::ItemFlags ObjectTypeModel::flags(const QModelIndex &index) const {
	Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

	if (index.isValid())
		return Qt::ItemIsDragEnabled | defaultFlags;
	else
		return defaultFlags;
}

ObjectType *ObjectTypeModel::getItem(const QModelIndex &index) const {
	return types.at(index.row());
}

void ObjectTypeModel::setObjectTypes(QList<QJsonObject> &list) {
	for (int i = 0; i < list.size(); i++) {
		types.append(new ObjectType(list[i]));
	}
}

QStringList ObjectTypeModel::mimeTypes() const {
	QStringList types;
	types << "application/eld-designer.object-type";
	return types;
}

QMimeData *ObjectTypeModel::mimeData(const QModelIndexList &indexes) const {
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			QString text = getItem(index)->type;
			stream << text;
		}
	}

	mimeData->setData("application/eld-designer.object-type", encodedData);
	return mimeData;
}
