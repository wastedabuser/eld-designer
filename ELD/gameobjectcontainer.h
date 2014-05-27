#ifndef GAMEOBJECTCONTAINER_H
#define GAMEOBJECTCONTAINER_H

#include <QWidget>

class GameObjectView;
class GameObject;

namespace Ui {
	class GameObjectContainer;
}

class GameObjectContainer : public QWidget
{
		Q_OBJECT

	public:
		explicit GameObjectContainer(QWidget *parent = 0);
		~GameObjectContainer();

		void paintEvent(QPaintEvent *);
		void addGameObject(GameObject *obj);
		void removeGameObject(GameObject *obj);
		GameObjectView *getView(GameObject *obj);

	public slots:
		void handleViewChange(GameObjectView *view = 0);

	private:
		Ui::GameObjectContainer *ui;
		QList<GameObjectView *> views;
		QHash<GameObject *, GameObjectView *> hViews;
};

#endif // GAMEOBJECTCONTAINER_H
