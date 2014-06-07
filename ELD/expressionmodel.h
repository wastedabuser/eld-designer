#ifndef EXPRESSIONMODEL_H
#define EXPRESSIONMODEL_H

#include "expression.h"

#include <QAbstractTableModel>

class ExpressionModel : public QAbstractTableModel
{
		Q_OBJECT
	public:
		explicit ExpressionModel(QString &expr, QObject *parent = 0);
		~ExpressionModel();

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

		Expression *getItem(const QModelIndex &index) const;
		void setExpressionData(QString &expr);
		QString getExpressionData();
		Expression *appendExpression();
		Expression *createExpression(QJsonArray &expr);
		Expression *removeExpression(const QModelIndex &index);

	signals:

	public slots:

	private:
		QList<Expression *> expressions;
};

#endif // EXPRESSIONMODEL_H
