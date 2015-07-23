#ifndef GAMEOBJECTCONTAINER_H
#define GAMEOBJECTCONTAINER_H

#include <QWidget>

class GameObjectView;
class GameObject;

namespace Ui {
	class GameObjectContainer;
}

class GameObjectContainer : public QWidget {
	Q_OBJECT

  public:
	explicit GameObjectContainer(QWidget *parent = 0);
	~GameObjectContainer();

	void paintEvent(QPaintEvent *);
	void wheelEvent(QWheelEvent *);
	void centerView();
	void addGameObject(GameObject *obj, bool doUpdate = true);
	void removeGameObject(GameObject *obj);
	void applyGameObjectsOrder(QList<GameObject *> &list);
	void updateCanvas();
	void selectGameObject(GameObject *obj);
	void selectView(GameObjectView *view);
	void zoom(bool out, bool wheel = false);
    void setScaleFactor(double sf);
    double getScaleFactor();
	QList<GameObjectView *> getViewsForObejcts(QList<GameObject *> &list);
	QRect getViewsBounds(QList<GameObjectView *> &list);

	double padding;
	double canvasPadding;

  public slots:
	void handleViewChange(GameObjectView *view = 0);

  signals:
	void gameObjectSelect(GameObject *obj);

  private:
	Ui::GameObjectContainer *ui;
	QList<GameObjectView *> views;
	QHash<GameObject *, GameObjectView *> hViews;
    double scaleFactor;
	double dmw;
	double dmh;
	double rx;
	double ry;

	void alignContainerCanvas();
};

#endif // GAMEOBJECTCONTAINER_H
