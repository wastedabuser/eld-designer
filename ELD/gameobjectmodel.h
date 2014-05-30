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

    GameObject *getItem(const QModelIndex &index) const;
	QString getNextGameObjectId(const QString &typeName);
    QJsonDocument getJson();
	QList<GameObject *> setJson(const QJsonDocument &doc);
	bool canCreateObject(const QString &typeName, const QModelIndex &index);
	GameObject *appendGameObjectFromJsonObject(const QJsonObject &obj, const QModelIndex &index);
	GameObject *createGameObject(const QString &typeName, const QModelIndex &index);
	GameObject *removeGameObject(const QModelIndex &index);
	bool moveGameObject(const QModelIndex &pindex, int offset);

  private:
    GameObject *rootItem;
	int itemIdCount;
};

#endif
