#include "config.h"
#include "propertymodel.h"
#include "gameobjectmodel.h"
#include "util.h"
#include "jsonio.h"

#include <QtGui>
#include <QJsonObject>

PropertyModel::PropertyModel(GameObject *gObject,const QString &id, const QString &typeName, const QJsonObject &obj, QObject *parent): QAbstractTableModel(parent) {
	gameObject = gObject;
    setJsonObject(id, typeName, obj);
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

        if (item->name == "_ID_" && !gameObject->idValid(value.toString()))
            return false;

		bool res = item->setData(index.column(), value);
		if (res) {
			emit dataChanged(index, index);
			emit propertyChanged(item->name, item->value);
			changesToCommit = true;
			if (!changeFlag) finishPropertyChange();

            if (item->systemProperty()) gameObject->reportIdChange();
		}
		return res;
    }
    return false;
}

Property *PropertyModel::getItem(const QModelIndex &index) const {
    return properties.at(index.row());
}

void PropertyModel::appendPropertyItem(QString &name, QString &value, QJsonObject &options) {
    properties.append(new Property(name, value, options));
}

void PropertyModel::updateFilesMap(QHash<QString, bool> &uniqueFiles) {
	for (int i = 0; i < properties.size(); i++) {
		Property *pr = properties[i];
        QString tp = pr->getType();
        if (tp == "file" || tp == "texture") uniqueFiles[pr->value] = true;
	}
}

QString PropertyModel::getId() {
    for (int i = 0; i < properties.size(); i++) {
        Property *pr = properties[i];
        if (pr->name == "_ID_") return pr->value;
    }
    return "";
}

QJsonObject PropertyModel::getJsonObject() {
	QJsonObject obj;
	for (int i = 0; i < properties.size(); i++) {
		Property *pr = properties[i];
        if (pr->systemProperty()) continue;
		obj[pr->name] = pr->value;
	}
	return obj;
}

void PropertyModel::setJsonObject(const QString &id, const QString &typeName, const QJsonObject &propData) {
	QList<QJsonObject> propList = Config::getPropertiesForType(typeName);
	QJsonObject propDefinition = Config::typesDefinitions[typeName];

    beginInsertRows(QModelIndex(), 0, propList.size() + 1);

	QJsonObject systemObj;
	QString systp = "_TYPE_";
	QString sysvl = typeName;
	systemObj["type"] = "readonly";
	systemObj["name"] = systp;
	appendPropertyItem(systp, sysvl, systemObj);

    QJsonObject systemObj1;
    QString systp1 = "_ID_";
    QString sysvl1 = id;
    systemObj1["type"] = "string";
    systemObj1["name"] = systp1;
    appendPropertyItem(systp1, sysvl1, systemObj1);

	for (int i = 0; i < propList.size(); ++i) {
		QJsonObject propertyObj = propList[i];
		QString propName = propertyObj["name"].toString();
		QString propVal = propData[propName].toString();
		QString defaultVal;
		if (propDefinition.contains(propName)) defaultVal = propDefinition[propName].toString();
		if (propVal.isEmpty()) propVal = defaultVal;

		appendPropertyItem(propName, propVal, propertyObj);
	}

	endInsertRows();
}

QString PropertyModel::getPropertyValue(const QString &name, const QString &typeName) {
	for (int i = 0; i < properties.size(); ++i) {
		if (properties[i]->name == name) return properties[i]->value;
	}
	QJsonObject propDefinition = Config::typesDefinitions[typeName];
	if (propDefinition.contains(name)) return propDefinition[name].toString();
	return QString();
}

bool PropertyModel::setPropertyValue(const QString &name, const QString &value, bool silent) {
	for (int i = 0; i < properties.size(); ++i) {
		if (properties[i]->name == name) {
			if (properties[i]->value != value) {
				properties[i]->value = value;
				emit dataChanged(QModelIndex().sibling(i,1), QModelIndex().sibling(i,1));
				if (!silent) emit propertyChanged(name, value);
				changesToCommit = true;
				if (!changeFlag) finishPropertyChange();
				return true;
			}
			return false;
		}
	}
	return false;
}

bool PropertyModel::hasProperty(const QString &name, const QString &typeName) {
	for (int i = 0; i < properties.size(); ++i) {
		if (properties[i]->name == name) return true;
	}
	QJsonObject propDefinition = Config::typesDefinitions[typeName];
	if (propDefinition.contains(name)) return true;
	return false;
}

void PropertyModel::setPropertyTrigger(const QString &propertyName, const QString &propertyValue, const QJsonObject &triggers) {
	QString name;
	QStringList tied = triggers.keys();
	for (int i = 0; i < tied.size(); i++) {
		QString k = tied[i];
		if (k.left(1) == "$") {
			name = k.right(k.length() - 1);
			if (name.isEmpty()) continue;

			if (propertyName == "reference" && triggers[k].isArray()) {
				QJsonDocument doc = JsonIO::readJson(Config::getResourceAbsolutePath(propertyValue));
				QJsonArray arr = triggers[k].toArray();
				QString value;
				for (int a = 0; a < arr.size(); a++) {
					value = JsonIO::findNode(doc, arr[a].toString()).toString();
					if (!value.isEmpty()) break;
				}
				if (setPropertyValue(name, value)) {
					Util::warn("Changed tied property " + name);
				}
			} else {
				QString value = triggers[k].toString();
				if (setPropertyValue(name, value)) {
					Util::warn("Changed tied property " + name);
				}
			}
		}
	}
}

void PropertyModel::startPropertyChange() {
	changeFlag = true;
	changesToCommit = false;
}

void PropertyModel::finishPropertyChange() {
	changeFlag = false;
	if (changesToCommit) {
		gameObject->gameObjectModel->onPropertyModelChanged(gameObject);
		changesToCommit = false;
	}
}
