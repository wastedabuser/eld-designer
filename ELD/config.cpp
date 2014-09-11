#include "config.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

QJsonArray Config::resourcePaths = QJsonArray();
QJsonArray Config::categoriesRef = QJsonArray();
QJsonArray Config::expressionsRef = QJsonArray();

QHash<QString, QJsonObject> Config::typesDefinitions = QHash<QString, QJsonObject>();
QHash<QString, QList<QJsonObject> > Config::typesListByCategory = QHash<QString, QList<QJsonObject> >();
QHash<QString, QJsonArray> Config::propertiesByObjectType = QHash<QString, QJsonArray>();
QHash<QString, QJsonObject> Config::propertiesDefinitions = QHash<QString, QJsonObject>();
QHash<QString, QJsonArray> Config::alowedChildObject = QHash<QString, QJsonArray>();
QHash<QString, QJsonObject> Config::expressionsDefinitions = QHash<QString, QJsonObject>();

void Config::setConfig(const QJsonDocument &configJson) {
	Config::typesDefinitions.clear();
	Config::typesListByCategory.clear();
	Config::propertiesByObjectType.clear();
	Config::propertiesDefinitions.clear();
	Config::alowedChildObject.clear();
	Config::expressionsDefinitions.clear();


	QJsonObject configObj = configJson.object();
	Config::resourcePaths = configObj["resourcePaths"].toArray();
	Config::categoriesRef = configObj["categories"].toArray();

	QJsonArray objectsArray = configObj["objects"].toArray();
	for (int i = 0; i < objectsArray.size(); ++i) {
		QJsonObject typeObj = objectsArray[i].toObject();

		QString typeName = typeObj["type"].toString();
		Config::typesDefinitions.insert(typeName, typeObj);
		Config::propertiesByObjectType.insert(typeName, typeObj["properties"].toArray());
		Config::alowedChildObject.insert(typeName, typeObj["children"].toArray());

		QString categoryType = typeObj["category"].toString();
		if (categoryType.isEmpty()) categoryType = "default";
		Config::typesListByCategory[categoryType].append(typeObj);
	}

	if (Config::typesListByCategory.isEmpty()) {
		QJsonObject defaultCat;
		defaultCat["type"] = "default";
		defaultCat["name"] = "Default objects";
		Config::typesListByCategory["default"].append(defaultCat);
	}

	QJsonArray propertiesArray = configObj["properties"].toArray();
	for (int i = 0; i < propertiesArray.size(); i++) {
		QJsonObject propObj = propertiesArray[i].toObject();

		QString propertyName = propObj["name"].toString();
		Config::propertiesDefinitions.insert(propertyName, propObj);
	}

	Config::expressionsRef = configObj["expressions"].toArray();
	for (int i = 0; i < Config::expressionsRef.size(); i++) {
		QJsonObject expr = Config::expressionsRef[i].toObject();

		QString exprName = expr["type"].toString();
		Config::expressionsDefinitions.insert(exprName, expr);
	}

}

QList<QJsonObject> Config::getPropertiesForType(const QString &typeName) {
	QJsonArray list = Config::propertiesByObjectType[typeName];
	QList<QJsonObject> propList;
	for (int i = 0; i < list.size(); ++i) {
		QString prop = list[i].toString();
		propList.append(Config::propertiesDefinitions[prop]);
	}
	return propList;
}

QHash<QString, bool> Config::getTypesForExpression(const QString &expr) {
	QHash<QString, bool> result;
	QJsonObject exprObj = Config::expressionsDefinitions[expr];
	QJsonArray whatList = exprObj["what"].toArray();
	for (int i = 0; i < whatList.size(); i++) {
		result[whatList[i].toString()] = true;
	}
	return result;
}

QList<QJsonObject> Config::getExpressionsForTypes(QHash<QString, bool> types) {
	QList<QJsonObject> result;
	for (int i = 0; i < Config::expressionsRef.size(); i++) {
		QJsonObject exprObj = Config::expressionsRef[i].toObject();
		if (!exprObj.contains("who")) {
			result.append(exprObj);
			continue;
		}
		QJsonArray whoList = exprObj["who"].toArray();
		for (int j = 0; j < whoList.size(); j++) {
			if (types.contains(whoList[j].toString())) result.append(exprObj);
		}
	}
	return result;
}

QString Config::getResourceAbsolutePath(const QString &path) {
	QString absolutePath;
	for (int i = 0; i < Config::resourcePaths.size(); i++) {
		absolutePath = Config::resourcePaths[i].toString() + path;
		QFile textureFile(absolutePath);
		if (textureFile.exists()) break;
	}
	return absolutePath;
}

QString Config::getResourceRelativePath(const QString &path) {
	QString relPath;
	for (int i = 0; i < Config::resourcePaths.size(); i++) {
		QString res = Config::resourcePaths[i].toString();
		if (path.left(res.length()) == res) {
			relPath = path.right(path.length() - res.length());
			break;
		}
	}
	return relPath;
}
