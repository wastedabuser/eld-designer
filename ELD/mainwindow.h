#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editor.h"
#include "objecttype.h"
#include "objecttypemodel.h"

#include <QJsonDocument>
#include <QListView>
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
	void loadConfig();
    void applySetting(const QString& key, const QString& value);
	void addRecentFile(const QString &fileName);
	void clearRecentFiles();
    void saveSettings();
	void updateRecentMenu();
	void copyGameObject(GameObject *obj);
	ObjectType *getSelectedObjectType();

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

	void on_actionClear_Recent_List_triggered();

	void on_recentFileOpenAction_triggered();

	void on_actionReload_config_file_triggered();

  private:
	Ui::MainWindow *ui;
    QString settingsFile;
    QJsonDocument settingsJson;
    QStringList types;
    int editorCnt;
	GameObject *copiedGameObject;
	QList<ObjectTypeModel *> objectTypeModels;
	QList<QListView *> listViews;

	void initObjectTypes();

};

#endif // MAINWINDOW_H
