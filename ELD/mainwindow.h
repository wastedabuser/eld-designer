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
    void addEditor(const QString& label, const QString &fileName = "");
    void readSettings();
    void applySetting(const QString& key, const QString& value);
    void saveSettings();
    ~MainWindow();

  private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionSelect_config_file_triggered();

    void on_actionSave_triggered();

  private:
    QString settingsFile;
    QJsonDocument settingsJson;

    QString configFile;
    QStringList types;

    int editorCnt;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
