#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QJsonArray>

class Expression {

  public:
	Expression(QJsonArray &expr);
	~Expression();

	QVariant data(int column) const;
	Qt::ItemFlags flags(int column) const;
	bool setData(int column, const QVariant &val);
	QJsonArray getJson();
	QString getValue(int column) const;
	QStringList getOptions(int column);

  private:
	QJsonArray expression;

};

#endif // EXPRESSION_H
