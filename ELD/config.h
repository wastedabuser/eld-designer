#ifndef CONFIG_H
#define CONFIG_H

#include <QHash>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class Config {

  public:
	static QJsonObject rootObject;
	static QJsonArray categoriesRef;
	static QJsonArray expressionsRef;
	static QHash<QString, QJsonArray> propertiesByObjectType;
	static QHash<QString, QJsonObject> typesDefinitions;
	static QHash<QString, QJsonObject> propertiesDefinitions;
	static QHash<QString, QJsonArray> alowedChildObject;
	static QHash<QString, QList<QJsonObject> > typesListByCategory;
	static QHash<QString, QJsonObject> expressionsDefinitions;

	static void setConfig(const QJsonDocument &configJson);
	static QList<QJsonObject> getPropertiesForType(const QString &typeName);
	static QHash<QString, bool> getTypesForExpression(const QString &expr);
	static QList<QJsonObject> getExpressionsForTypes(QHash<QString, bool> types);
};

#endif // CONFIG_H
