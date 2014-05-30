#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editor.h"

#include <QJsonDocument>
#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
    void addEditor(const QString& label, const QString &fileName = "");
    void readSettings();
    void applySetting(const QString& key, const QString& value);
    void saveSettings();
	void copyGameObject(GameObject *obj);

  private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionSelect_config_file_triggered();

    void on_actionSave_triggered();

	void on_actionZoom_in_triggered();

	void on_actionZoom_out_triggered();

	void on_actionCopy_triggered();

	void on_actionPaste_triggered();

	void on_actionCut_triggered();

  private:
	Ui::MainWindow *ui;
    QString settingsFile;
    QJsonDocument settingsJson;
    QString configFile;
    QStringList types;
    int editorCnt;
	GameObject *copiedGameObject;

};

#endif // MAINWINDOW_H
