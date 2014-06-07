#ifndef EXPRESSIONEDITORDELEGATE_H
#define EXPRESSIONEDITORDELEGATE_H

#include <QStyledItemDelegate>

class ExpressionEditorDelegate : public QStyledItemDelegate {
  public:
	ExpressionEditorDelegate();
	~ExpressionEditorDelegate();

	virtual QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void destroyEditor(QWidget * editor, const QModelIndex & index) const;
	virtual void setEditorData ( QWidget *editor, const QModelIndex &index ) const;
	virtual void setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
};

#endif // EXPRESSIONEDITORDELEGATE_H
