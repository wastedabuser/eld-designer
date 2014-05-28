#include "config.h"

#include <QJsonArray>
#include <QJsonObject>

QJsonObject Config::rootObject = QJsonObject();
QJsonObject Config::propertiesRef = QJsonObject();
QStringList Config::typesList = QStringList();
QHash<QString, QJsonArray> Config::propertiesByObjectType = QHash<QString, QJsonArray>();
QHash<QString, QJsonObject> Config::typesDefinitions = QHash<QString, QJsonObject>();
QHash<QString, QJsonArray> Config::alowedChildObject = QHash<QString, QJsonArray>();

void Config::setConfig(const QJsonDocument &configJson) {
	QJsonObject configObj = configJson.object();
	Config::rootObject = configObj["rootObject"].toObject();
	QJsonArray objectsArray = configObj["objects"].toArray();
	Config::propertiesRef = configObj["properties"].toObject();

	Config::alowedChildObject.insert(rootObject["type"].toString(), rootObject["children"].toArray());

	Config::typesList.clear();
	for (int i = 0; i < objectsArray.size(); ++i) {
		QJsonObject typeObj = objectsArray[i].toObject();
		QString typeName = typeObj["type"].toString();
		Config::typesList.append(typeName);
		Config::typesDefinitions.insert(typeName, typeObj);
		Config::propertiesByObjectType.insert(typeName, typeObj["properties"].toArray());
		Config::alowedChildObject.insert(typeName, typeObj["children"].toArray());
	}

}

QList<QJsonObject> Config::getPropertiesForType(const QString &typeName) {
	QJsonArray list = Config::propertiesByObjectType[typeName];
	QList<QJsonObject> propList;
	for (int i = 0; i < list.size(); ++i) {
		QString prop = list[i].toString();
		propList.append(Config::propertiesRef[prop].toObject());
	}
	return propList;
}
