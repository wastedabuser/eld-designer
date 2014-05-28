#ifndef GAMEOBJECTVIEW_H
#define GAMEOBJECTVIEW_H

#include "gameobjectcontainer.h"
#include "gameobject.h"
#include "hoverpoints.h"

#include <QWidget>
#include <QBasicTimer>
#include <QPolygonF>

class GameObjectView : public QObject {
	Q_OBJECT

  public:
	GameObjectView(GameObjectContainer *widget, GameObject *obj);
	~GameObjectView();

	void renderView();
	void setDelta(int dx, int dy);
	void setRotation(qreal r);
	void setZoomChange(double sf);
	QRect bouds();
	void commitProperties();
	void fetchProperties();

	bool selected;

  public slots:
	void updateCtrlPoints(const QPolygonF &);
	void updateComplete();
	void propertyModelUpdated(const QString &, const QString &);

  signals:
	void viewChanged(GameObjectView *view);

  private:
	GameObjectContainer *container;
	GameObject *gameObject;
	HoverPoints *pts;
	QPolygonF ctrlPoints;
	QImage image;
	int width;
	int height;
	double rotation;
	double scaleFactor;
};

#endif // GAMEOBJECTVIEW_H
