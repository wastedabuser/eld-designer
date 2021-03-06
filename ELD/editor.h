#ifndef EDITOR_H
#define EDITOR_H

#include "gameobjectcontainer.h"
#include "gameobjectmodel.h"

#include <QWidget>
#include <QJsonObject>

class MainWindow;

namespace Ui {
	class Editor;
}

class Editor : public QWidget {
	Q_OBJECT

  public:
	explicit Editor(MainWindow *mainW, QWidget *parent = 0);
	~Editor();

	QList<QJsonObject> getPropertiesForType(const QString &typeName);
    QString type();
    void save(bool saveAs);
	void load(const QString &fileNm);
	void createNew(QJsonObject &obj);
	void addNode(const QModelIndex &index);
	void zoomIn();
	void zoomOut();

	GameObject *copyGameObject();
	GameObject *cutGameObject();
	void pasteGameObject(GameObject *obj);
	void applyGameObjectsOrder();

	int tabIndex;
	QString fileName;

  private slots:
	void onEditorReady();
	void onGameObjectChanged();
	void onGameObjectAdded(GameObject *obj);
	void onGameObjectRemoved(GameObject *obj);

	void on_gameObject_selected(GameObject *obj);

	void on_addNode_clicked();

	void on_removeNode_clicked();

	void on_treeView_clicked(const QModelIndex &index);

	void on_moveNodeUp_clicked();

	void on_moveNodeDown_clicked();

	void on_editRoot_clicked();

  private:
	Ui::Editor *ui;
	MainWindow *mainWindow;
	GameObjectModel *gameObjectModel;
	GameObjectContainer *gameObjectContainer;

};

#endif // EDITOR_H
