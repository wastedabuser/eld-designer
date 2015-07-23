#include "expressiondesigner.h"
#include "expressioneditordelegate.h"
#include "expressionmodel.h"
#include "ui_expressiondesigner.h"

ExpressionDesigner *ExpressionDesigner::dialog;

QString ExpressionDesigner::getExpression(QString expr, GameObjectModel *gObjectModel, QWidget *parent) {
	ExpressionDesigner::dialog = new ExpressionDesigner(expr, gObjectModel, parent);
	int code = ExpressionDesigner::dialog->exec();

	QString data;
	if (code == Rejected)
		data = expr;
	else
		data = ExpressionDesigner::dialog->getExpressionData();

	delete ExpressionDesigner::dialog;
	return data;
}

ExpressionDesigner::ExpressionDesigner(QString expr, GameObjectModel *gObjectModel, QWidget *parent) : QDialog(parent),
	ui(new Ui::ExpressionDesigner) {
	ui->setupUi(this);

	setExpression(expr);
	gameObjectModel = gObjectModel;
}

ExpressionDesigner::~ExpressionDesigner() {
	delete ui;
}

void ExpressionDesigner::setExpression(QString &expr) {
	expressionModel = new ExpressionModel(expr);
	ui->tableView->setModel(expressionModel);
	ui->tableView->setItemDelegate(new ExpressionEditorDelegate());
    ui->tableView->setColumnWidth(0, 150);
    ui->tableView->setColumnWidth(1, 500);
}

QString ExpressionDesigner::getExpressionData() {
	return expressionModel->getExpressionData();
}

void ExpressionDesigner::on_addExpression_clicked() {
	expressionModel->appendExpression();
}

void ExpressionDesigner::on_removeExpression_clicked() {
	QTableView *view = ui->tableView;
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return;

	Expression *item = expressionModel->removeExpression(index);

	view->selectionModel()->clearCurrentIndex();
	view->clearSelection();

	delete item;
}
