#include "config.h"
#include "propertymodel.h"
#include "util.h"

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

		triggerPropertyChanged(item);
        return true;
    }
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
	QJsonObject propDefinition = Config::typesDefinitions[typeName];
	if (propList.isEmpty()) return;

	beginInsertRows(QModelIndex(), 0, propList.size() - 1);
	for (int i = 0; i < propList.size(); ++i) {
		QJsonObject propertyObj = propList[i];
		QString propName = propertyObj["name"].toString();
		QString propVal = propData[propName].toString();
		QString defaultVal;
		if (propDefinition.contains(propName)) defaultVal = propDefinition[propName].toString();

		if (defaultVal.left(1) == "$") tieProperty(defaultVal.right(defaultVal.length() - 1), propName);
		else if (propVal.isEmpty()) propVal = defaultVal;

		appendPropertyItem(propName, propVal, propertyObj);
	}
	endInsertRows();
}

QString PropertyModel::getPropertyValue(const QString &name, const QString &typeName) {
	QString _name = name;
	if (tiedProperties.contains(name)) _name = tiedProperties[name];

	for (int i = 0; i < properties.size(); ++i) {
		if (properties[i]->name == _name) return properties[i]->value;
	}
	QJsonObject propDefinition = Config::typesDefinitions[typeName];
	if (propDefinition.contains(_name)) return propDefinition[_name].toString();
	return QString();
}

void PropertyModel::setPropertyValue(const QString &name, const QString &value) {
	QString _name = name;
	if (tiedProperties.contains(name)) _name = tiedProperties[name];

	for (int i = 0; i < properties.size(); ++i) {
		if (properties[i]->name == _name) {
			properties[i]->value = value;
			emit dataChanged(QModelIndex().sibling(i,1), QModelIndex().sibling(i,1));
			return;
		}
	}
}

bool PropertyModel::hasProperty(const QString &name, const QString &typeName) {
	QString _name = name;
	if (tiedProperties.contains(name))_name = tiedProperties[name];

	for (int i = 0; i < properties.size(); ++i) {
		if (properties[i]->name == _name) return true;
	}
	QJsonObject propDefinition = Config::typesDefinitions[typeName];
	if (propDefinition.contains(_name)) return true;
	return false;
}

void PropertyModel::tieProperty(const QString &name, const QString &toName) {
	tiedProperties[name] = toName;
}

void PropertyModel::triggerPropertyChanged(Property *item) {
	QString name = item->name;
	if (!tiedProperties.isEmpty()) {
		QList<QString> tied = tiedProperties.keys();
		for (int i = 0; i < tied.size(); i++) {
			if (tiedProperties[tied[i]] == name) {
				name = tied[i];
				break;
			}
		}
	}
	Util::warn("Changed " + name);
	emit propertyChanged(name, item->value);
}
