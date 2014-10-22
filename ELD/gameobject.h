#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "gameobjectview.h"

#include <QList>
#include <QVariant>
#include <QVector>

class GameObjectModel;
class PropertyModel;

class GameObject {

  public:
	GameObject(GameObjectModel *model, const QJsonObject &obj, GameObject *parent = 0);

    ~GameObject();

	GameObject *child(int number);
	QList<GameObject *> getChildrenListDeep();
	QList<GameObject *> getParentsList();
	QList<GameObject *> getChildren();
	GameObject *cloneDeep();
    int childCount() const;
    int columnCount() const;
	int childNumber() const;
	QVariant data() const;
	GameObject *parent();

	GameObject *appendChild(const QJsonObject &obj, GameObjectModel *model);
	void removeChild(int index);
	void addChild(int index, GameObject *obj);
	QJsonArray getChildJsonArray();
	QJsonObject getJsonObject();
	void updateFilesMap(QHash<QString, bool> &uniqueFiles);
	void createChildrenFromJsonArray(const QJsonArray &list);
	void createPropertiesFromJsonObject(const QJsonObject &obj);
	QString getPropertyValue(const QString &name);
	void setPropertyValue(const QString &name, const QString &value);
	bool hasProperty(const QString &name);
	bool hasView();

	QString type;
	QString id;
	GameObjectModel *gameObjectModel;
    PropertyModel *propertyModel;

  signals:
	void propertyModelChanged();

  private:
	GameObject *parentItem;
	QList<GameObject *> childItems;

};

#endif
