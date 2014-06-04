#ifndef OBJECTTYPEMODEL_H
#define OBJECTTYPEMODEL_H

#include "objecttype.h"

#include <QAbstractListModel>

class ObjectTypeModel : public QAbstractListModel
{
		Q_OBJECT
	public:
		explicit ObjectTypeModel(QList<QJsonObject> &list, QObject *parent = 0);
		~ObjectTypeModel();

		int rowCount(const QModelIndex &parent) const;
		QVariant data(const QModelIndex &index, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QStringList mimeTypes() const;
		QMimeData *mimeData(const QModelIndexList &indexes) const;

		ObjectType *getItem(const QModelIndex &index) const;
		void setObjectTypes(QList<QJsonObject> &list);

	signals:

	public slots:

	private:
		QList<ObjectType *> types;

};

#endif // OBJECTTYPEMODEL_H
