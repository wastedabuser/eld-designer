#include "expressioneditordelegate.h"
#include "expressionmodel.h"

#include <QComboBox>
#include <QCompleter>

ExpressionEditorDelegate::ExpressionEditorDelegate() {

}

ExpressionEditorDelegate::~ExpressionEditorDelegate() {

}

QWidget* ExpressionEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	ExpressionModel* model = (ExpressionModel*) index.model();
	Expression *prop = model->getItem(index);
	QStringList options = prop->getOptions(index.column());

	QCompleter *completer = new QCompleter(parent);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	QComboBox *cb = new QComboBox(parent);
	cb->setCompleter(completer);
	cb->setEditable(true);
	for (int i = 0; i < options.size(); i++) {
		QString obj = options[i];
		cb->addItem(obj);
	}
	return cb;
}

void ExpressionEditorDelegate::destroyEditor(QWidget * editor, const QModelIndex & index) const {

	QStyledItemDelegate::destroyEditor(editor, index);
}

void ExpressionEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	QString currentText = index.data(Qt::EditRole).toString();
	if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
		int cbIndex = cb->findText(currentText);
		if(cbIndex >= 0)
			cb->setCurrentIndex(cbIndex);
	} else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void ExpressionEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
		ExpressionModel *pm = (ExpressionModel*) model;
		pm->setData(index, cb->currentText(), Qt::EditRole);
	} else
		QStyledItemDelegate::setModelData(editor, model, index);
}
