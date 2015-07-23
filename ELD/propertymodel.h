#ifndef PROPERTYMODEL_H
#define PROPERTYMODEL_H

#include "gameobject.h"
#include "property.h"

#include <QAbstractTableModel>
#include <QJsonObject>

class PropertyModel : public QAbstractTableModel {
    Q_OBJECT

  public:
    PropertyModel(GameObject *gObject, const QString &id, const QString &typeName, const QJsonObject &obj, QObject *parent = 0);
    ~PropertyModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Property *getItem(const QModelIndex &index) const;
    void createPropertyItems(const QList<QJsonObject> &propList, const QJsonObject &propData);
	void appendPropertyItem(QString &name, QString &value, QJsonObject &options);
	void updateFilesMap(QHash<QString, bool> &uniqueFiles);
    QString getId();
	QJsonObject getJsonObject();
    void setJsonObject(const QString &id, const QString &typeName, const QJsonObject &propData);
	QString getPropertyValue(const QString &name, const QString &typeName);
	bool setPropertyValue(const QString &name, const QString &value, bool silent = false);
	bool hasProperty(const QString &name, const QString &typeName);
	void setPropertyTrigger(const QString &propertyName, const QString &propertyValue, const QJsonObject &triggers);
	void startPropertyChange();
	void finishPropertyChange();

	GameObject *gameObject;

  signals:
	void propertyChanged(const QString &name, const QString &value);

  private:
    QList<Property*> properties;
	bool changeFlag;
	bool changesToCommit;

};

#endif // PROPERTYMODEL_H
