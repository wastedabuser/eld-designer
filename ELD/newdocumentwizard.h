#ifndef NEWDOCUMENTWIZARD_H
#define NEWDOCUMENTWIZARD_H

#include <QDialog>

namespace Ui {
	class NewDocumentWizard;
}

class NewDocumentWizard : public QDialog {
	Q_OBJECT

  public:
	explicit NewDocumentWizard(QWidget *parent = 0);

	static NewDocumentWizard *dialog;
	static QJsonObject getDocumentJsonObject(QWidget *parent = 0);

	~NewDocumentWizard();

	QJsonObject getJson();

  private:
	Ui::NewDocumentWizard *ui;
};

#endif // NEWDOCUMENTWIZARD_H
