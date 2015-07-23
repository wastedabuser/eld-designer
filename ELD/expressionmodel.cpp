#include "expressionmodel.h"

#include <QJsonDocument>

ExpressionModel::ExpressionModel(QString &expr, QObject *parent) : QAbstractTableModel(parent) {
	setExpressionData(expr);
}

ExpressionModel::~ExpressionModel() {
	qDeleteAll(expressions);
}

int ExpressionModel::rowCount(const QModelIndex &parent) const {
	return expressions.count();
}

int ExpressionModel::columnCount(const QModelIndex & /* parent */) const {
	return 3;
}

QVariant ExpressionModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	Expression *item = getItem(index);

    if (role == Qt::DisplayRole)
        return item->view(index.column());

	return item->data(index.column());
}

Qt::ItemFlags ExpressionModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	Expression *item = getItem(index);
	return QAbstractItemModel::flags(index) | item->flags(index.column());
}

bool ExpressionModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole) {

		Expression *item = getItem(index);
		item->setData(index.column(), value);
		emit dataChanged(index, index);

		return true;
	}
	return false;
}

Expression *ExpressionModel::getItem(const QModelIndex &index) const {
	return expressions.at(index.row());
}

void ExpressionModel::setExpressionData(QString &expr) {
	if (expr.isEmpty()) return;

	QJsonDocument doc = QJsonDocument::fromJson(expr.toUtf8());
	QJsonArray list = doc.array();
	if (list.isEmpty()) return;

	beginInsertRows(QModelIndex(), 0, list.size() - 1);
	for (int i = 0; i < list.size(); i++) {
		QJsonArray arr = list[i].toArray();
		createExpression(arr);
	}
	endInsertRows();
}

QString ExpressionModel::getExpressionData() {
	QJsonDocument doc;
	QJsonArray list;
	for (int i = 0; i < expressions.size(); i++) {
		list.append(expressions[i]->getJson());
	}
	doc.setArray(list);
	return QString(doc.toJson(QJsonDocument::Compact));
}

Expression *ExpressionModel::appendExpression() {
	QJsonArray expr;
	beginInsertRows(QModelIndex(), expressions.size(), expressions.size());
	Expression *item = createExpression(expr);
	endInsertRows();
	return item;
}

Expression *ExpressionModel::createExpression(QJsonArray &expr) {
	Expression *item = new Expression(expr);
	expressions.append(item);
	return item;
}

Expression *ExpressionModel::removeExpression(const QModelIndex &index) {
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	Expression *item = expressions.takeAt(index.row());
	endRemoveRows();
	return item;
}
