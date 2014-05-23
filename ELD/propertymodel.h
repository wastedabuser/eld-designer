#include "property.h"

#include <QAbstractTableModel>
#include <QJsonObject>

#ifndef PROPERTYMODEL_H
#define PROPERTYMODEL_H

class PropertyModel : public QAbstractTableModel {
    Q_OBJECT

  public:
	PropertyModel(const QString &typeName, const QJsonObject &obj, QObject *parent = 0);
    ~PropertyModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Property *getItem(const QModelIndex &index) const;
    void createPropertyItems(const QList<QJsonObject> &propList, const QJsonObject &propData);
    void appendPropertyItem(QString &name, QString &value, QJsonObject &options);
	QJsonObject getJsonObject();
	void setJsonObject(const QString &typeName, const QJsonObject &propData);

  private:
    QList<Property*> properties;

};

#endif // PROPERTYMODEL_H
