#include "config.h"
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
#include <QCheckBox>

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
		cb->addItem("", QVariant(QJsonObject()));
		for (int i = 0; i < options.size(); i++) {
			QJsonObject obj = options[i].toObject();
			cb->addItem(obj["type"].toString(), QVariant(obj));
		}
		return cb;
	} else if (propertyType == "check") {
		QCheckBox *cb = new QCheckBox(parent);

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
	QString path = Config::getResourceAbsolutePath(lineEditor->text());
	path = QFileDialog::getOpenFileName(actionBtn, tr("Pick file"), path, "Files (" + propertyMeta + ")");
	if (!path.isEmpty()) {
		lineEditor->setText(Config::getResourceRelativePath(path));
		emit commitData(editorContainer);
	}
}

void PropertyEditorDelegate::on_colorBtn_clicked() {
	QColor color = QColorDialog::getColor(QColor(lineEditor->text()),actionBtn);
	if (color.isValid()) {
		lineEditor->setText(color.name());
		emit commitData(editorContainer);
	}
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
	PropertyModel *pm = (PropertyModel*) model;
	Property *prop = propertyModel->getItem(index);
	if (propertyType == "file" || propertyType == "color" || propertyType == "expression") {
		pm->startPropertyChange();
		model->setData(index, lineEditor->text());
		pm->setPropertyTrigger(prop->name, lineEditor->text(), prop->options);
		pm->finishPropertyChange();
	} else if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
		pm->startPropertyChange();
		pm->setData(index, cb->currentText(), Qt::EditRole);
		pm->setPropertyTrigger(prop->name, cb->currentText(), cb->currentData().toJsonObject());
		pm->finishPropertyChange();
	} else
		QStyledItemDelegate::setModelData(editor, model, index);
}
