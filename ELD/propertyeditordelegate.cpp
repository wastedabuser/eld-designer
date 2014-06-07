#include "expressiondesigner.h"
#include "propertyeditordelegate.h"
#include "propertymodel.h"
#include <QComboBox>
#include <QJsonArray>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QColorDialog>

PropertyEditorDelegate::PropertyEditorDelegate(QObject *parent): QStyledItemDelegate(parent) {

}

PropertyEditorDelegate::~PropertyEditorDelegate() {

}

QWidget* PropertyEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	propertyModel = (PropertyModel*) index.model();
	Property *prop = propertyModel->getItem(index);
	propertyMeta = prop->getMeta();
	propertyType = prop->getType();

	if (propertyType == "combo") {
		QComboBox *cb = new QComboBox(parent);
		QJsonArray options = prop->getOptions();
		for (int i = 0; i < options.size(); i++) {
			QJsonObject obj = options[i].toObject();
			cb->addItem(obj["type"].toString(), QVariant(obj));
		}
		return cb;
	} else if (propertyType == "file" || propertyType == "color" || propertyType == "expression") {
		editorContainer = new QWidget(parent);
		QHBoxLayout *layout = new QHBoxLayout;
		lineEditor = new QLineEdit;
		actionBtn = new QPushButton("...");

		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(1);
		layout->addWidget(lineEditor);
		layout->addWidget(actionBtn);
		editorContainer->setLayout(layout);

		if (propertyType == "file") connect(actionBtn, SIGNAL(clicked()), this, SLOT(on_fileBtn_clicked()));
		else if (propertyType == "color") connect(actionBtn, SIGNAL(clicked()), this, SLOT(on_colorBtn_clicked()));
		else if (propertyType == "expression") connect(actionBtn, SIGNAL(clicked()), this, SLOT(on_expressionBtn_clicked()));

		return editorContainer;
	}

	return QStyledItemDelegate::createEditor(parent, option, index);
}

void PropertyEditorDelegate::on_fileBtn_clicked() {
	QString path = QFileDialog::getOpenFileName(actionBtn, tr("Pick file"), "", "Files (" + propertyMeta + ")");
	lineEditor->setText(path);
	emit commitData(editorContainer);
}

void PropertyEditorDelegate::on_colorBtn_clicked() {
	QColor color = QColorDialog::getColor(QColor(lineEditor->text()),actionBtn);
	lineEditor->setText(color.name());
	emit commitData(editorContainer);
}

void PropertyEditorDelegate::on_expressionBtn_clicked() {
	QString expression = ExpressionDesigner::getExpression(lineEditor->text(), propertyModel->gameObject->gameObjectModel, actionBtn);
	lineEditor->setText(expression);
	emit commitData(editorContainer);
}

void PropertyEditorDelegate::destroyEditor(QWidget * editor, const QModelIndex & index) const {
	if (propertyType == "file") disconnect(actionBtn, SIGNAL(clicked()), this, SLOT(on_fileBtn_clicked()));
	QStyledItemDelegate::destroyEditor(editor, index);
}

void PropertyEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	QString currentText = index.data(Qt::EditRole).toString();
	if (propertyType == "file" || propertyType == "color" || propertyType == "expression") {
		lineEditor->setText(currentText);
	} else if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
		int cbIndex = cb->findText(currentText);
		if(cbIndex >= 0)
			cb->setCurrentIndex(cbIndex);
	} else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void PropertyEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if (propertyType == "file" || propertyType == "color" || propertyType == "expression") {
		model->setData(index, lineEditor->text());
	} else if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
		PropertyModel *pm = (PropertyModel*) model;
		pm->setData(index, cb->currentText(), Qt::EditRole);
		pm->setPropertyTrigger(cb->currentData().toJsonObject());
	} else
		QStyledItemDelegate::setModelData(editor, model, index);
}
