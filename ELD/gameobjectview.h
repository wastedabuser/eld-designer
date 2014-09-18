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
	GameObjectView(GameObjectContainer *widget, GameObject *obj, double scaleF = 1, int x = 0, int y = 0);
	~GameObjectView();

	QRect bouds();
	void renderView();
	void select(bool state);
	void setDelta(int dx, int dy);
	void setRotation(qreal r);
	void setZoomChange(double sf);
	void propagateViewChange();
	void commitPositionProperties();
	void fetchSizeProperties();
	void fetchRotationProperty();
	void fetchPositionProperties();
	void fetchShapeProperty();
	void fetchTextureProperty();
	void cacheRelatedViews();

	bool selected;
	GameObject *gameObject;

  public slots:
	void onPointsChangeStart();
	void onPointsChanged(const QPolygonF &);
	void onPointsChangeComplete();
	void onPropertyModelUpdated(const QString &, const QString &);

	void onPolylineChangeComplete();

  signals:
	void viewChanged(GameObjectView *view);

  private:
	GameObjectContainer *container;
	QList<GameObjectView *> childViews;
	QList<GameObjectView *> parentViews;
	QImage image;
	QRectF rectangle;
	HoverPoints *polyline;

	HoverPoints *hoverPoints;
	QPolygonF controlPolygon;

	bool hasPosition;
	int width;
	int height;
	double zoomFactor;
	double rotation;
	bool hasRotation;
	bool hasBgColor;
	QColor bgColor;
	bool hasAlpha;
	bool hasParallax;
	double alpha;
	double parallax;
	bool hasSize;

};

#endif // GAMEOBJECTVIEW_H
