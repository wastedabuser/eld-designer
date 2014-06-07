#ifndef EXPRESSIONDESIGNER_H
#define EXPRESSIONDESIGNER_H

#include "expressionmodel.h"
#include "gameobjectmodel.h"

#include <QDialog>

namespace Ui {
	class ExpressionDesigner;
}

class ExpressionDesigner : public QDialog {
	Q_OBJECT

  public:
	explicit ExpressionDesigner(QString expr, GameObjectModel *gObjectModel, QWidget *parent = 0);
	~ExpressionDesigner();

	static ExpressionDesigner *dialog;
	static QString getExpression(QString expr, GameObjectModel *gObjectModel, QWidget *parent = 0);

	void setExpression(QString &expr);
	QString getExpressionData();
	GameObjectModel *gameObjectModel;

  private slots:
	void on_addExpression_clicked();

	void on_removeExpression_clicked();

  private:
	Ui::ExpressionDesigner *ui;
	ExpressionModel *expressionModel;

};

#endif // EXPRESSIONDESIGNER_H
