#include "config.h"
#include "newdocumentwizard.h"
#include "ui_newdocumentwizard.h"

#include <QJsonObject>

NewDocumentWizard *NewDocumentWizard::dialog;

QJsonObject NewDocumentWizard::getDocumentJsonObject(QWidget *parent) {
	NewDocumentWizard::dialog = new NewDocumentWizard(parent);
	int code = NewDocumentWizard::dialog->exec();

	QJsonObject data;
	if (code != Rejected)
		data = NewDocumentWizard::dialog->getJson();

	delete NewDocumentWizard::dialog;
	return data;
}

NewDocumentWizard::NewDocumentWizard(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NewDocumentWizard) {
	ui->setupUi(this);

	QList<QJsonObject> objects = Config::typesListByCategory["rootObject"];
	for (int i = 0; i < objects.size(); i++) {
		QJsonObject obj = objects[i];
		ui->comboBox->addItem(obj["type"].toString());
	}
}

NewDocumentWizard::~NewDocumentWizard() {
	delete ui;
}

QJsonObject NewDocumentWizard::getJson() {
	QJsonObject obj;
	obj["id"] = ui->lineEdit->text();
	obj["type"] = ui->comboBox->currentText();
	return obj;
}
