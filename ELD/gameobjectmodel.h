#ifndef GAMEOBJECTMODEL_H
#define GAMEOBJECTMODEL_H

#include "gameobject.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QJsonDocument>

class GameObjectModel : public QAbstractItemModel {
    Q_OBJECT

  public:
	GameObjectModel(QObject *parent = 0);
    ~GameObjectModel();

    QVariant data(const QModelIndex &index, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	QStringList mimeTypes() const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	QModelIndex getGameObjectIndex(GameObject *obj) const;
    GameObject *getItem(const QModelIndex &index) const;
	QString getNextGameObjectId(const QString &typeName);
    QJsonDocument getJson();
	void setJson(const QJsonDocument &doc);
	void setJsonObject(const QJsonObject &obj);
	bool canCreateObject(const QString &typeName, const QModelIndex &index);
	GameObject *appendGameObjectFromJsonObject(const QJsonObject &obj, const QModelIndex &index);
	GameObject *createGameObject(const QString &typeName, const QModelIndex &index);
	GameObject *removeGameObject(const QModelIndex &index);
	bool moveGameObject(const QModelIndex &pindex, int offset);
	void onPropertyModelChanged(GameObject *obj);

	GameObject *rootItem;
	QHash<QString, GameObject *> objectsById;

  signals:
	void gameObjectChanged();
	void gameObjectAdded(GameObject *);

  private:
	int idCounter;
};

#endif
