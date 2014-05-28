#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "gameobjectview.h"
#include "propertymodel.h"

#include <QList>
#include <QVariant>
#include <QVector>

class GameObject {
  public:
	GameObject(const QJsonObject &obj, GameObject *parent = 0);

    ~GameObject();

    GameObject *child(int number);
	QList<GameObject *> getChildrenListDeep();
    int childCount() const;
    int columnCount() const;
	int childNumber() const;
	QVariant data() const;
	GameObject *parent();

	GameObject *appendChild(const QJsonObject &obj);
	void removeChild(int index);
	QJsonArray getChildJsonArray();
	QJsonObject getJsonObject();
	void createChildrenFromJsonArray(const QJsonArray &list);
	void createPropertiesFromJsonObject(const QJsonObject &obj);
	QString getPropertyValue(const QString &name);
	void setPropertyValue(const QString &name, const QString &value);
	bool hasView();

	QString type;
	QString id;
    PropertyModel *propertyModel;

  private:
	GameObject *parentItem;
	QList<GameObject *> childItems;

};

#endif
