#ifndef EXPRESSIONEDITORDELEGATE_H
#define EXPRESSIONEDITORDELEGATE_H

#include "expressionmodel.h"

#include <QPushButton>
#include <QStyledItemDelegate>

class ExpressionEditorDelegate : public QStyledItemDelegate {
    Q_OBJECT

  public:
    ExpressionEditorDelegate(QObject *parent = 0);
	~ExpressionEditorDelegate();

	virtual QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void destroyEditor(QWidget * editor, const QModelIndex & index) const;
	virtual void setEditorData ( QWidget *editor, const QModelIndex &index ) const;
	virtual void setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

  public slots:
    void on_fileBtn_clicked();

  private:
    mutable ExpressionModel* expressionModel;
    mutable QString expressionType;
    mutable QString expressionMeta;

    mutable QWidget *editorContainer;
    mutable QPushButton *actionBtn;
    mutable QLineEdit *lineEditor;

};

#endif // EXPRESSIONEDITORDELEGATE_H
