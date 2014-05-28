#ifndef FILEPROPERTYDELEGATE_H
#define FILEPROPERTYDELEGATE_H

#include <QStyledItemDelegate>

class FilePropertyDelegate : public QStyledItemDelegate {
	Q_OBJECT

  public:
	FilePropertyDelegate(QObject *parent = 0);
	~FilePropertyDelegate();

	virtual QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void setEditorData ( QWidget *editor, const QModelIndex &index ) const;
	virtual void setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

	QString propertyMeta;
};

#endif // FILEPROPERTYDELEGATE_H
