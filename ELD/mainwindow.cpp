#include "config.h"
#include "editor.h"
#include "jsonio.h"
#include "mainwindow.h"
#include "newdocumentwizard.h"
#include "objecttypemodel.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QFileDialog>
#include <QListView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow) {
    ui->setupUi(this);

    editorCnt = 1;

	QTabWidget *tabs = ui->tabWidget;
	tabs->clear();

    settingsFile = "settings.json";
    readSettings();

	copiedGameObject = 0;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::readSettings() {
    settingsJson = JsonIO::readJson(settingsFile);
    if (settingsJson.isNull()) return;

	loadConfig();
	updateRecentMenu();
}

void MainWindow::loadConfig() {
	QJsonObject obj = settingsJson.object();
	QString configFile = obj["configFile"].toString();
	if (configFile.isEmpty()) return;

	QJsonDocument configJson = JsonIO::readJson(configFile);
	Config::setConfig(configJson);
	Util::warn("Config loaded ", configFile);

	initObjectTypes();
}

void MainWindow::initObjectTypes() {
	QJsonArray categories = Config::categoriesRef;
	for (int i = 0; i < categories.size(); i++) {
		QJsonObject cat = categories[i].toObject();
		QListView *listView = new QListView();
		ObjectTypeModel *model = new ObjectTypeModel(Config::typesListByCategory[cat["type"].toString()]);
		listViews.append(listView);
		objectTypeModels.append(model);
		listView->setModel(model);
		listView->setDragEnabled(true);
		QWidget *w = ui->toolBox->widget(i);
		if (w) {
			QLayout *l = w->layout();
			if (l) delete l;
			QVBoxLayout *layout = new QVBoxLayout();
			layout->setMargin(0);
			layout->addWidget(listView);
			w->setLayout(layout);
			ui->toolBox->setItemText(i, cat["name"].toString());
		} else {
			ui->toolBox->addItem(listView, cat["name"].toString());
		}
	}
}

ObjectType *MainWindow::getSelectedObjectType() {
	int i = ui->toolBox->currentIndex();
	QListView *view = listViews.at(i);
	QModelIndex index = view->selectionModel()->currentIndex();
	if (!index.isValid()) return 0;

	ObjectTypeModel *model = objectTypeModels.at(i);
	return model->getItem(index);
}

void MainWindow::applySetting(const QString &key, const QString &value) {
    QJsonObject obj = settingsJson.object();
    obj[key] = value;
    settingsJson.setObject(obj);
    saveSettings();
}

void MainWindow::addRecentFile(const QString &fileName) {
	if (fileName.isEmpty()) return;

	QJsonObject obj = settingsJson.object();
	QJsonArray recent;
	if (obj.contains("recent")) {
		recent = obj["recent"].toArray();
		for (int i = 0; i < recent.size(); i++) {
			if (recent[i].toString() == fileName) recent.removeAt(i);
		}
	}
	recent.prepend(fileName);
	obj["recent"] = recent;
	settingsJson.setObject(obj);
	saveSettings();
	updateRecentMenu();
}

void MainWindow::clearRecentFiles() {
	QJsonObject obj = settingsJson.object();
	obj["recent"] = QJsonArray();
	settingsJson.setObject(obj);
	saveSettings();
	updateRecentMenu();
}

void MainWindow::updateRecentMenu() {
	QJsonObject obj = settingsJson.object();
	QJsonArray recent;
	if (obj.contains("recent")) recent = obj["recent"].toArray();

	ui->menuOpen_Recent->clear();
	for (int i = 0; i < recent.size(); i++) {
		QAction *openFileAction = ui->menuOpen_Recent->addAction(recent[i].toString());
		connect(openFileAction, SIGNAL(triggered()), this, SLOT(on_recentFileOpenAction_triggered()));
	}
	ui->menuOpen_Recent->addSeparator();
	QAction *clearRecent = ui->menuOpen_Recent->addAction(tr("Clear Recent"));
	connect(clearRecent, SIGNAL(triggered()), this, SLOT(on_actionClear_Recent_List_triggered()));
}

void MainWindow::saveSettings() {
    JsonIO::writeJson(settingsFile, settingsJson);
}

QString MainWindow::lastOpenDir() {
	return settingsJson.object()["lastOpenDir"].toString();
}

void MainWindow::updateEditorIndexes() {
	QTabWidget *tabs = ui->tabWidget;
	for (int i = 0; i < tabs->count(); i++) {
		Editor *curEditor = (Editor*) tabs->widget(i);
		curEditor->tabIndex = i;
	}
}

void MainWindow::addEditor(const QString &label, const QString &fileName) {
	QTabWidget *tabs = ui->tabWidget;

	Editor *editor = new Editor(this);
	if (fileName.isEmpty()) {
		QJsonObject obj = NewDocumentWizard::getDocumentJsonObject(this);
		if (obj.isEmpty()) return;
		editor->createNew(obj);
	} else
		editor->load(fileName);

    tabs->addTab(editor, "[" + editor->type() + "] " + label);
    tabs->setCurrentWidget(editor);
	editor->tabIndex = tabs->currentIndex();
}


void MainWindow::copyGameObject(GameObject *obj) {
	if (!obj) return;
	if (copiedGameObject) delete copiedGameObject;
	copiedGameObject = obj;
}

void MainWindow::on_actionOpen_triggered() {
	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"), lastOpenDir(), tr("Files (*.txt;*.json)"));

	for (int i = 0; i < fileNames.size(); i++) {
		QString fileName = fileNames[i];
		QFileInfo info1(fileName);
		addRecentFile(fileName);

		QString lastOpenDir = info1.absolutePath();
		applySetting("lastOpenDir", lastOpenDir);

        MainWindow::addEditor(info1.fileName(), fileName);
	}
}

void MainWindow::on_actionNew_triggered() {
    QString label = "New ";
    label.append(QString::number(editorCnt));
    editorCnt++;
    MainWindow::addEditor(label);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index) {
    QTabWidget *tabs = ui->tabWidget;
	tabs->removeTab(index);
}

void MainWindow::on_actionSelect_config_file_triggered() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.txt;*.json)"));
	if (fileName.isEmpty()) return;

    QString key = "configFile";
    applySetting(key, fileName);
	loadConfig();
}

void MainWindow::on_actionSave_triggered() {
    QTabWidget *tabs = ui->tabWidget;
    if (tabs->currentWidget() == 0) return;

    Editor *curEditor = (Editor*) tabs->currentWidget();
    curEditor->save(false);
}

void MainWindow::on_actionSave_As_triggered() {
    QTabWidget *tabs = ui->tabWidget;
    if (tabs->currentWidget() == 0) return;

    Editor *curEditor = (Editor*) tabs->currentWidget();
    curEditor->save(true);
}

void MainWindow::on_actionZoom_in_triggered() {
	QTabWidget *tabs = ui->tabWidget;
	if (tabs->currentWidget() == 0) return;

	Editor *curEditor = (Editor*) tabs->currentWidget();
	curEditor->zoomIn();
}

void MainWindow::on_actionZoom_out_triggered() {
	QTabWidget *tabs = ui->tabWidget;
	if (tabs->currentWidget() == 0) return;

	Editor *curEditor = (Editor*) tabs->currentWidget();
	curEditor->zoomOut();
}

void MainWindow::on_actionCopy_triggered() {
	QTabWidget *tabs = ui->tabWidget;
	if (tabs->currentWidget() == 0) return;

	Editor *curEditor = (Editor*) tabs->currentWidget();
	copyGameObject(curEditor->copyGameObject());
}

void MainWindow::on_actionPaste_triggered() {
	if (!copiedGameObject) return;

	QTabWidget *tabs = ui->tabWidget;
	if (tabs->currentWidget() == 0) return;

	Editor *curEditor = (Editor*) tabs->currentWidget();
	curEditor->pasteGameObject(copiedGameObject);
}

void MainWindow::on_actionCut_triggered() {
	QTabWidget *tabs = ui->tabWidget;
	if (tabs->currentWidget() == 0) return;

	Editor *curEditor = (Editor*) tabs->currentWidget();
	copyGameObject(curEditor->cutGameObject());
}

void MainWindow::on_actionClear_Recent_List_triggered() {
	clearRecentFiles();
}

void MainWindow::on_recentFileOpenAction_triggered() {
	QAction *action = qobject_cast<QAction *>(sender());
	QString fileName = action->text();
	QFileInfo info1(fileName);
	MainWindow::addEditor(info1.fileName(), fileName);
}

void MainWindow::on_actionReload_config_file_triggered() {
	loadConfig();
}

void MainWindow::on_actionClose_triggered() {
	QTabWidget *tabs = ui->tabWidget;
	tabs->removeTab(tabs->currentIndex());
}

void MainWindow::on_actionSave_All_triggered() {
	QTabWidget *tabs = ui->tabWidget;
	for (int i = 0; i < tabs->count(); i++) {
		Editor *curEditor = (Editor*) tabs->widget(i);
        curEditor->save(false);
	}
}

void MainWindow::on_actionClose_All_triggered() {
	QTabWidget *tabs = ui->tabWidget;
	tabs->clear();
}
