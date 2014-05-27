#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

#include "gameobject.h"
#include "propertymodel.h"

GameObject::GameObject(const QJsonObject &obj, GameObject *parent) {
	parentItem = parent;

	if (obj.isEmpty()) return;

	type = obj["type"].toString();
	id = obj["id"].toString();
	propertyModel = new PropertyModel(type, obj["properties"].toObject());

	QJsonArray children = obj["children"].toArray();
	if (!children.isEmpty()) createChildrenFromJsonArray(children);
}

GameObject::~GameObject() {
	qDeleteAll(childItems);
	delete propertyModel;
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
	obj["type"] = type;
	obj["id"] = id;
	QJsonArray children = getChildJsonArray();
	if (!children.isEmpty()) obj["children"] = children;
	obj["properties"] = propertyModel->getJsonObject();
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

QString GameObject::getPropertyValue(const QString &name) {
	return propertyModel->getPropertyValue(name, type);
}

void GameObject::setPropertyValue(const QString &name, const QString &value) {
	propertyModel->setPropertyValue(name, value);
}

bool GameObject::hasView() {
	return propertyModel->hasProperty("texture", type) || propertyModel->hasProperty("shape", type);
}
