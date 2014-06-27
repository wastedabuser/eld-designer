#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

#include "config.h"
#include "gameobject.h"
#include "propertymodel.h"
#include "gameobjectmodel.h"

GameObject::GameObject(GameObjectModel *model, const QJsonObject &obj, GameObject *parent) {
	gameObjectModel = model;
	parentItem = parent;

	type = obj["type"].toString();
	id = obj["id"].toString();
	propertyModel = new PropertyModel(this, type, obj["properties"].toObject());

	if (gameObjectModel) gameObjectModel->objectsById[id] = this;

	QJsonArray children = obj["children"].toArray();
	if (!children.isEmpty()) createChildrenFromJsonArray(children);

}

GameObject::~GameObject() {
	qDeleteAll(childItems);
	if (propertyModel) delete propertyModel;
}

QList<GameObject *> GameObject::getChildrenListDeep() {
	QList<GameObject *> list;
	for (int i = 0; i < childItems.size(); i++) {
		QList<GameObject *> chList = childItems[i]->getChildrenListDeep();
		for (int j = 0; j < chList.size(); j++) {
			list.append(chList[j]);
		}
		list.append(childItems[i]);
	}
	return list;
}

QList<GameObject *> GameObject::getParentsList() {
	QList<GameObject *> list;
	if (parentItem) {
		list.append(parentItem);
		QList<GameObject *> pList = parentItem->getParentsList();
		for (int j = 0; j < pList.size(); j++) {
			list.append(pList[j]);
		}
	}
	return list;
}

QList<GameObject *> GameObject::getChildren() {
	QList<GameObject *> list(childItems);
	return list;
}

GameObject *GameObject::cloneDeep() {
	return new GameObject(0, getJsonObject());
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
		appendChild(list[i].toObject(), gameObjectModel);
	}
}

GameObject *GameObject::appendChild(const QJsonObject &obj, GameObjectModel *model) {
	GameObject *item = new GameObject(model, obj, this);
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
	if (!propertyModel) return QString();
	return propertyModel->getPropertyValue(name, type);
}

void GameObject::setPropertyValue(const QString &name, const QString &value) {
	if (!propertyModel) return;
	if (propertyModel->setPropertyValue(name, value)) gameObjectModel->onPropertyModelChanged(this);
}

bool GameObject::hasProperty(const QString &name) {
	if (!propertyModel) return false;
	return propertyModel->hasProperty(name, type);
}

bool GameObject::hasView() {
	if (!propertyModel) return false;
	return propertyModel->hasProperty("texture", type) || propertyModel->hasProperty("shape", type);
}
