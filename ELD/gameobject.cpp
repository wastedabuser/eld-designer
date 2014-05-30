#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

#include "config.h"
#include "gameobject.h"
#include "propertymodel.h"

GameObject::GameObject(const QJsonObject &obj, GameObject *parent) {
	parentItem = parent;

	if (obj.isEmpty()) {
		type = Config::rootObject["type"].toString();
	} else {
		type = obj["type"].toString();
		id = obj["id"].toString();
		propertyModel = new PropertyModel(type, obj["properties"].toObject());

		QJsonArray children = obj["children"].toArray();
		if (!children.isEmpty()) createChildrenFromJsonArray(children);
	}
}

GameObject::~GameObject() {
	qDeleteAll(childItems);
	delete propertyModel;
}

QList<GameObject *> GameObject::getChildrenListDeep() {
	QList<GameObject *> list;
	for (int i = 0; i < childItems.size(); i++) {
		list.append(childItems[i]);
		QList<GameObject *> chList = childItems[i]->getChildrenListDeep();
		for (int j = 0; j < chList.size(); j++) {
			list.append(chList[j]);
		}
	}
	return list;
}

GameObject *GameObject::child(int number) {
	return childItems.value(number);
}

int GameObject::childCount() const {
	return childItems.count();
}

int GameObject::childNumber() const {
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<GameObject*>(this));

	return 0;
}

GameObject *GameObject::parent() {
	return parentItem;
}

QJsonArray GameObject::getChildJsonArray() {
	QJsonArray list;
	for (int i = 0; i < childItems.size(); i++) {
		list.append(childItems[i]->getJsonObject());
	}
	return list;
}

QJsonObject GameObject::getJsonObject() {
	QJsonObject obj;
	if (!type.isEmpty()) obj["type"] = type;
	if (!id.isEmpty()) obj["id"] = id;
	QJsonArray children = getChildJsonArray();
	if (!children.isEmpty()) obj["children"] = children;
	if (propertyModel) {
		QJsonObject properties = propertyModel->getJsonObject();
		if (!properties.isEmpty()) obj["properties"] = properties;
	}
	return obj;
}

void GameObject::createChildrenFromJsonArray(const QJsonArray &list) {
	for (int i = 0; i < list.size(); i++) {
		appendChild(list[i].toObject());
	}
}

GameObject *GameObject::appendChild(const QJsonObject &obj) {
	GameObject *item = new GameObject(obj, this);
	childItems.append(item);
	return item;
}

void GameObject::removeChild(int index) {
	childItems.removeAt(index);
}

void GameObject::addChild(int index, GameObject *obj) {
	childItems.insert(index, obj);
}

QString GameObject::getPropertyValue(const QString &name) {
	return propertyModel->getPropertyValue(name, type);
}

void GameObject::setPropertyValue(const QString &name, const QString &value) {
	propertyModel->setPropertyValue(name, value);
}

bool GameObject::hasProperty(const QString &name) {
	return propertyModel->hasProperty(name, type);
}

bool GameObject::hasView() {
	return propertyModel->hasProperty("texture", type) || propertyModel->hasProperty("shape", type);
}
