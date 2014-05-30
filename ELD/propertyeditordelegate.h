#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QPushButton>
#include <QStyledItemDelegate>

class PropertyEditorDelegate : public QStyledItemDelegate {
	Q_OBJECT

  public:
	PropertyEditorDelegate(QObject *parent = 0);
	~PropertyEditorDelegate();

	virtual QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void destroyEditor(QWidget * editor, const QModelIndex & index) const;
	virtual void setEditorData ( QWidget *editor, const QModelIndex &index ) const;
	virtual void setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

	mutable QString propertyType;
	mutable QString propertyMeta;

	mutable QWidget *editorContainer;
	mutable QPushButton *fileBtn;
	mutable QLineEdit *lineEditor;

  public slots:
	void on_fileBtn_clicked();
	void on_colorBtn_clicked();

};

#endif // ITEMDELEGATE_H
