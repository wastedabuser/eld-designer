#include "config.h"

#include <QJsonArray>
#include <QJsonObject>

QHash<QString, QJsonArray> Config::propertiesByObjectType = QHash<QString, QJsonArray>();
QJsonObject Config::propertiesRef = QJsonObject();
QStringList Config::typesList = QStringList();

void Config::setConfig(const QJsonDocument &configJson) {
	QJsonObject configObj = configJson.object();
	QJsonArray objectsArray = configObj["objects"].toArray();

	Config::typesList.clear();
	for (int i = 0; i < objectsArray.size(); ++i) {
		QJsonObject typeObj = objectsArray[i].toObject();
		QString typeName = typeObj["type"].toString();
		Config::typesList.append(typeName);
		Config::propertiesByObjectType.insert(typeName, typeObj["properties"].toArray());
	}
	Config::propertiesRef = configObj["properties"].toObject();
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
