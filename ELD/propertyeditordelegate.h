#ifndef PROPERTYEDITORDELEGATE_H
#define PROPERTYEDITORDELEGATE_H

#include "propertymodel.h"

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

  public slots:
	void on_fileBtn_clicked();
	void on_colorBtn_clicked();
	void on_expressionBtn_clicked();

  private:
	mutable PropertyModel* propertyModel;
	mutable QString propertyType;
	mutable QString propertyMeta;

	mutable QWidget *editorContainer;
	mutable QPushButton *actionBtn;
	mutable QLineEdit *lineEditor;
};

#endif // PROPERTYEDITORDELEGATE_H
