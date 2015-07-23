#include "config.h"
#include "expressioneditordelegate.h"
#include "expressionmodel.h"
#include <QCompleter>
#include <QComboBox>
#include <QJsonArray>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>

ExpressionEditorDelegate::ExpressionEditorDelegate(QObject *parent): QStyledItemDelegate(parent) {

}

ExpressionEditorDelegate::~ExpressionEditorDelegate() {

}

QWidget* ExpressionEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    expressionModel = (ExpressionModel*) index.model();
    Expression *prop = expressionModel->getItem(index);
    int col = index.column();
    if (col == 2) {
        expressionType = prop->getType();
        expressionMeta = prop->getMeta();
    } else {
        expressionType = "";
        expressionMeta = "";
    }

    if (col != 2 || expressionType.isEmpty()) {
        QString value = prop->getValue(col);
        QList<QStringList> options = prop->getOptions(col);

        bool shouldCreateList = value.isEmpty();
        if (!shouldCreateList) {
            for (int i = 0; i < options.size(); i++) {
                QStringList obj = options[i];
                if (obj[0] == value || obj[1] == value) {
                    shouldCreateList = true;
                    break;
                }
            }
        }
        if (shouldCreateList) {
            QCompleter *completer = new QCompleter(parent);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            QComboBox *cb = new QComboBox(parent);
            cb->setCompleter(completer);
            cb->setEditable(true);
            cb->addItem(QString());
            for (int i = 0; i < options.size(); i++) {
                QStringList obj = options[i];
                cb->addItem(obj[0], obj[1]);
            }
            return cb;
        }
    } else if (expressionType == "string" || expressionType == "number") {
        QLineEdit *le = new QLineEdit(parent);

        return le;
    } else {
        editorContainer = new QWidget(parent);
        QHBoxLayout *layout = new QHBoxLayout;
        lineEditor = new QLineEdit;
        actionBtn = new QPushButton("...");

        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(1);
        layout->addWidget(lineEditor);
        layout->addWidget(actionBtn);
        editorContainer->setLayout(layout);

        if (expressionType == "file") connect(actionBtn, SIGNAL(clicked()), this, SLOT(on_fileBtn_clicked()));

        return editorContainer;
    }

	return QStyledItemDelegate::createEditor(parent, option, index);
}

void ExpressionEditorDelegate::on_fileBtn_clicked() {
    QString path = Config::getResourceAbsolutePath(lineEditor->text());
    path = QFileDialog::getOpenFileName(actionBtn, tr("Pick file"), path, "Files (" + expressionMeta + ")");
    if (!path.isEmpty()) {
        lineEditor->setText(Config::getResourceRelativePath(path));
        emit commitData(editorContainer);
    }
}

void ExpressionEditorDelegate::destroyEditor(QWidget * editor, const QModelIndex & index) const {
    if (expressionType == "file") disconnect(actionBtn, SIGNAL(clicked()), this, SLOT(on_fileBtn_clicked()));
	QStyledItemDelegate::destroyEditor(editor, index);
}

void ExpressionEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QString currentData = index.data(Qt::EditRole).toString();
    if (expressionType == "file") {
        lineEditor->setText(currentData);
    } else if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        int cbIndex = cb->findData(currentData);
        if (cbIndex >= 0)
			cb->setCurrentIndex(cbIndex);
    } else if (QLineEdit *cb = qobject_cast<QLineEdit *>(editor)) {
        cb->setText(currentData);
    } else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void ExpressionEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    ExpressionModel *pm = (ExpressionModel*) model;
    if (expressionType == "file") {
        pm->setData(index, lineEditor->text());
    } else if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        QVariant dt = cb->currentData();
        if (dt.isNull()) pm->setData(index, cb->currentText());
        else pm->setData(index, dt);
    } else if (QLineEdit *cb = qobject_cast<QLineEdit *>(editor)) {
        pm->setData(index, cb->text());
    } else
		QStyledItemDelegate::setModelData(editor, model, index);
}
