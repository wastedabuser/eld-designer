#ifndef GAMEOBJECTVIEW_H
#define GAMEOBJECTVIEW_H

#include "gameobjectcontainer.h"
#include "gameobject.h"
#include "hoverpoints.h"
#include "property.h"

#include <QWidget>
#include <QBasicTimer>
#include <QPolygonF>

class GameObjectView : public QObject {
	Q_OBJECT

  public:
	GameObjectView(GameObjectContainer *widget, GameObject *obj);
	~GameObjectView();

	void drawPixmap();
	void applyDelta(int dx, int dy);
	QRect bouds();
	void commitProperties();
	void fetchProperties();

  public slots:
	void updateCtrlPoints(const QPolygonF &);
	void updateComplete();
	void propertyModelUpdated(Property *);

  signals:
	void viewChanged(GameObjectView *view);

  private:
	GameObjectContainer *container;
	GameObject *gameObject;
	HoverPoints *pts;
	QPolygonF ctrlPoints;
	QImage pixmap;
	int width;
	int height;
	double m_rotation;
	double m_scale;
	double m_shear;
};

#endif // GAMEOBJECTVIEW_H
