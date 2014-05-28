#ifndef CONFIG_H
#define CONFIG_H

#include <QHash>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class Config {

  public:
	static QJsonObject rootObject;
	static QJsonObject propertiesRef;
	static QStringList typesList;
	static QHash<QString, QJsonArray> propertiesByObjectType;
	static QHash<QString, QJsonObject> typesDefinitions;
	static QHash<QString, QJsonArray> alowedChildObject;

	static void setConfig(const QJsonDocument &configJson);
	static QList<QJsonObject> getPropertiesForType(const QString &typeName);
};

#endif // CONFIG_H
