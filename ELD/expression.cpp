#include "config.h"
#include "expression.h"
#include "expressiondesigner.h"

#include <QVariant>

Expression::Expression(QJsonArray &expr) {
	expression = expr;
	if (expression.isEmpty()) {
		for (int i = 0; i < 3; i++) {
			expression.append("");
		}
	}
}

Expression::~Expression() {

}

QVariant Expression::data(int column) const {
	return expression[column].toVariant();
}

Qt::ItemFlags Expression::flags(int column) const {
	return Qt::ItemIsEditable;
}

bool Expression::setData(int column, const QVariant &val) {
	expression[column] = QVariant(val).toString();
	return true;
}

QJsonArray Expression::getJson() {
	return expression;
}

QString Expression::getValue(int column) const {
	return expression[column].toString();
}

QStringList Expression::getOptions(int column) {
	QStringList result;
	if (column == 0) {
		QList<GameObject *> list = ExpressionDesigner::dialog->gameObjectModel->rootItem->getChildrenListDeep();
		for (int i = 0; i < list.size(); i++) {
			result.append(list[i]->id);
		}
	} else if (column == 1) {
		QList<GameObject *> list = ExpressionDesigner::dialog->gameObjectModel->rootItem->getChildrenListDeep();
		QHash<QString, bool> hTypes;
		for (int i = 0; i < list.size(); i++) {
			if (list[i]->id == expression[0].toString()) hTypes[list[i]->type] = true;
		}
		QList<QJsonObject> exps = Config::getExpressionsForTypes(hTypes);
		for (int i = 0; i < exps.size(); i++) {
			QJsonObject obj = exps[i];
			result.append(obj["type"].toString());
		}
	} else if (column == 2) {
		QHash<QString, bool> lkp = Config::getTypesForExpression(expression[1].toString());
		QList<GameObject *> list = ExpressionDesigner::dialog->gameObjectModel->rootItem->getChildrenListDeep();
		for (int i = 0; i < list.size(); i++) {
			if (lkp.contains(list[i]->type)) result.append(list[i]->id);
		}
	}
	return result;
}
