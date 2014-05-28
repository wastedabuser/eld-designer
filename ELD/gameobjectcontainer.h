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
		void wheelEvent(QWheelEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void addGameObject(GameObject *obj);
		void removeGameObject(GameObject *obj);
		void selectGameObject(GameObject *obj);
		void zoom(bool out, bool wheel = false);

	public slots:
		void handleViewChange(GameObjectView *view = 0);

	private:
		Ui::GameObjectContainer *ui;
		QList<GameObjectView *> views;
		QHash<GameObject *, GameObjectView *> hViews;
		double canvasPadding;
		double dmw;
		double dmh;
		double rx;
		double ry;

		void alignContainerCanvas();
};

#endif // GAMEOBJECTCONTAINER_H
