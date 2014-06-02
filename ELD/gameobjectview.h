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
	void createRelatedViewsBoundingRectangle();
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
	QList<GameObjectView *> relatedViews;
	QImage image;
	QRectF rectangle;
	HoverPoints *polyline;

	HoverPoints *hoverPoints;
	QPolygonF controlPolygon;

	int width;
	int height;
	double zoomFactor;
	double rotation;
	bool canRotate;
	bool hasBgColor;
	QColor bgColor;
	bool hasAlpha;
	double alpha;
};

#endif // GAMEOBJECTVIEW_H
