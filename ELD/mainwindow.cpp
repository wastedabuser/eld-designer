#include "config.h"
#include "editor.h"
#include "jsonio.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow) {
    ui->setupUi(this);

    editorCnt = 1;

    QTabWidget *tabs = ui->tabWidget;
    tabs->removeTab(0);
    tabs->removeTab(0);

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

    QJsonObject obj = settingsJson.object();
    configFile = obj["configFile"].toString();
	QJsonDocument configJson = JsonIO::readJson(configFile);
	Config::setConfig(configJson);
    Util::warn("Config loaded ", configFile);
}

void MainWindow::applySetting(const QString &key, const QString &value) {
    QJsonObject obj = settingsJson.object();
    obj[key] = value;
    settingsJson.setObject(obj);
    saveSettings();
}

void MainWindow::saveSettings() {
    JsonIO::writeJson(settingsFile, settingsJson);
}

void MainWindow::addEditor(const QString &label, const QString &fileName) {
	QTabWidget *tabs = ui->tabWidget;

	Editor *editor = new Editor();
	editor->load(fileName);

    tabs->addTab(editor, label);
    tabs->setCurrentWidget(editor);
	editor->tabIndex = tabs->currentIndex();
}


void MainWindow::copyGameObject(GameObject *obj) {
	if (!obj) return;
	if (copiedGameObject) delete copiedGameObject;
	copiedGameObject = obj;
}

void MainWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.txt;*.json;*.lvjson;*.aijson)"));
    if (fileName.isEmpty()) return;

    QFileInfo info1(fileName);
    MainWindow::addEditor(info1.fileName(), fileName);
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.json)"));
    QString key = "configFile";
    applySetting(key, fileName);
}

void MainWindow::on_actionSave_triggered() {
    QTabWidget *tabs = ui->tabWidget;
    if (tabs->currentWidget() == 0) return;

    Editor *curEditor = (Editor*) tabs->currentWidget();
    curEditor->save();
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
