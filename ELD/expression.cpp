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

QVariant Expression::view(int column) const {
    if (column == 1) {
        QString key = expression[column].toString();
        QJsonObject prop = Config::expressionsDefinitions[key];
        if (!prop.isEmpty()) return createView(prop);
    }
    return expression[column].toString();
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

QString Expression::getType() const {
    QString key = expression[1].toString();
    if (key.isEmpty()) return "";

    QJsonObject prop = Config::expressionsDefinitions[key];
    return prop["type"].toString();
}

QString Expression::getMeta() const {
    QString key = expression[1].toString();
    if (key.isEmpty()) return "";

    QJsonObject prop = Config::expressionsDefinitions[key];
    return prop["meta"].toString();
}

QString Expression::createView(QJsonObject prop) const {
    QString key = prop["name"].toString();
    QStringList types;
    if (prop.contains("what")) {
        QJsonArray whats = prop["what"].toArray();
        for (int i = 0; i < whats.size(); i++) {
            types.append(whats[i].toString());
        } 
    }
    if (prop.contains("type")) types.append(prop["type"].toString());
    if (types.size() > 0) key += "(" + types.join("/") + ")";
    if (prop.contains("description")) key += "; " + prop["description"].toString();
    return key;
}

QList<QStringList> Expression::getOptions(int column) {
    QList<QStringList> result;
	if (column == 0) {
		QList<GameObject *> list = ExpressionDesigner::dialog->gameObjectModel->rootItem->getChildrenListDeep();
		for (int i = 0; i < list.size(); i++) {
            QStringList it;
            it.append(list[i]->id);
            it.append(list[i]->id);
            result.append(it);
		}
	} else if (column == 1) {
        QString who = expression[0].toString();
        QHash<QString, bool> hTypes;
        if (who == "") {
            hTypes[ExpressionDesigner::dialog->gameObjectModel->rootItem->type] = true;
        } else {
            QList<GameObject *> list = ExpressionDesigner::dialog->gameObjectModel->rootItem->getChildrenListDeep();
            for (int i = 0; i < list.size(); i++) {
                if (list[i]->id == who) hTypes[list[i]->type] = true;
            }
        }
		QList<QJsonObject> exps = Config::getExpressionsForTypes(hTypes);
		for (int i = 0; i < exps.size(); i++) {
			QJsonObject obj = exps[i];
            QStringList it;
            it.append(createView(obj));
            it.append(obj["name"].toString());
            result.append(it);
		}
	} else if (column == 2) {
		QHash<QString, bool> lkp = Config::getTypesForExpression(expression[1].toString());
		QList<GameObject *> list = ExpressionDesigner::dialog->gameObjectModel->rootItem->getChildrenListDeep();
		for (int i = 0; i < list.size(); i++) {
            if (lkp.contains(list[i]->type)) {
                QStringList it;
                it.append(list[i]->id);
                it.append(list[i]->id);
                result.append(it);
            }
		}
	}
	return result;
}
