#ifndef EDITOR_H
#define EDITOR_H

#include "gameobjectcontainer.h"
#include "gameobjectmodel.h"

#include <QWidget>
#include <QJsonObject>

namespace Ui {
	class Editor;
}

class Editor : public QWidget {
	Q_OBJECT

  public:
	explicit Editor(QWidget *parent = 0);
	~Editor();

	QList<QJsonObject> getPropertiesForType(const QString &typeName);
	void save();
	void load(const QString &fileNm);
	void addNode(const QModelIndex &index);
	void zoomIn();
	void zoomOut();

	GameObject *copyGameObject();
	GameObject *cutGameObject();
	void pasteGameObject(GameObject *obj);

	int tabIndex;
	QString fileName;

  private slots:
	void on_addNode_clicked();

	void on_removeNode_clicked();

	void on_treeView_clicked(const QModelIndex &index);

	void on_addRootNode_clicked();

	void on_moveNodeUp_clicked();

	void on_moveNodeDown_clicked();

	void on_expandTree_clicked();

  private:
	Ui::Editor *ui;
	GameObjectModel *gameObjectModel;
	GameObjectContainer *gameObjectContainer;
};

#endif // EDITOR_H
