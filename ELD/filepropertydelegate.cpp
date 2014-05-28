#include "filepropertydelegate.h"
#include "propertymodel.h"

#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>

FilePropertyDelegate::FilePropertyDelegate(QObject *parent): QStyledItemDelegate(parent) {

}


FilePropertyDelegate::~FilePropertyDelegate() {

}


QWidget* FilePropertyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	PropertyModel* model = (PropertyModel*) index.model();
	Property *prop = model->getItem(index);

	if (prop->getType() != "file") {
		return QStyledItemDelegate::createEditor(parent, option, index);
	}

	//propertyMeta = prop->getMeta();
	//QFileDialog::getSaveFileName(this, tr("Open File"), "", tr("Files (*.txt;*.json;*.lvjson;*.aijson)"));

	QWidget *cb = new QWidget(parent);
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(new QLineEdit());
	layout->addWidget(new QPushButton("..."));
	cb->setLayout(layout);

	return cb;
}



void FilePropertyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	if(QWidget *cb = qobject_cast<QWidget *>(editor)) {
		QString currentText = index.data(Qt::EditRole).toString();

	} else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}


void FilePropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if(QWidget *cb = qobject_cast<QWidget *>(editor)) {
		//model->setData(index, cb->currentData(), Qt::EditRole);
	} else
		QStyledItemDelegate::setModelData(editor, model, index);
}
