#ifndef QMIMEDATAWITHINDEX_H
#define QMIMEDATAWITHINDEX_H

#include <QMimeData>
#include <QModelIndexList>

class QMimeDataWithIndex : public QMimeData
{
	public:
		QMimeDataWithIndex();
		QModelIndexList indexList;
};

#endif // QMIMEDATAWITHINDEX_H
