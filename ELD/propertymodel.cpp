#include "config.h"
#include "propertymodel.h"

#include <QtGui>
#include <QJsonObject>

PropertyModel::PropertyModel(const QString &typeName, const QJsonObject &obj, QObject *parent): QAbstractTableModel(parent) {
	setJsonObject(typeName, obj);
}

PropertyModel::~PropertyModel() {
	qDeleteAll(properties);
}

int PropertyModel::rowCount(const QModelIndex &parent) const {
    return properties.count();
}

int PropertyModel::columnCount(const QModelIndex & /* parent */) const {
    return 2;
}

QVariant PropertyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    Property *item = getItem(index);
    return item->data(index.column());
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    Property *item = getItem(index);
    return QAbstractItemModel::flags(index) | item->flags(index.column());
}

bool PropertyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == Qt::EditRole) {

        Property *item = getItem(index);
        item->setData(index.column(), value);

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool PropertyModel::insertRows (int row, int count, const QModelIndex & parent) {
    //properties.insert(new Property(name, data, options));
    return false;
}


Property *PropertyModel::getItem(const QModelIndex &index) const {
    return properties.at(index.row());
}

void PropertyModel::appendPropertyItem(QString &name, QString &value, QJsonObject &options) {
    properties.append(new Property(name, value, options));
}

QJsonObject PropertyModel::getJsonObject() {
	QJsonObject obj;
	for (int i = 0; i < properties.size(); i++) {
		obj[properties[i]->name] = properties[i]->value;
	}
	return obj;
}

void PropertyModel::setJsonObject(const QString &typeName, const QJsonObject &propData) {
	QList<QJsonObject> propList = Config::getPropertiesForType(typeName);
	if (propList.isEmpty()) return;

	beginInsertRows(QModelIndex(), 0, propList.size() - 1);
	for (int i = 0; i < propList.size(); ++i) {
		QJsonObject propertyObj = propList[i];
		QString propName = propertyObj["name"].toString();
		QString propVal = propData[propName].toString();
		appendPropertyItem(propName, propVal, propertyObj);
	}
	endInsertRows();
}

