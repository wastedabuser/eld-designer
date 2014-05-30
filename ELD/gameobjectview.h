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
	GameObjectView(GameObjectContainer *widget, GameObject *obj, double scaleF = 1);
	~GameObjectView();

	void renderView();
	void setDelta(int dx, int dy);
	void setRotation(qreal r);
	void setZoomChange(double sf);
	QRect bouds();
	void commitPositionProperties();
	void fetchSizeProperties();
	void fetchRotationProperty();
	void fetchPositionProperties();
	void fetchShapeProperty();
	void fetchTextureProperty();
	void cacheRelatedViews();

	bool selected;

  public slots:
	void onPointsChangeStart();
	void onPointsChanged(const QPolygonF &);
	void onPointsChangeComplete();
	void onPropertyModelUpdated(const QString &, const QString &);

  signals:
	void viewChanged(GameObjectView *view);

  private:
	GameObjectContainer *container;
	GameObject *gameObject;
	HoverPoints *pts;
	QList<GameObjectView *> relatedViews;
	QPolygonF ctrlPoints;
	QImage image;
	QRectF rectangle;
	int width;
	int height;
	double scaleFactor;
	double rotation;
	bool canRotate;
	bool hasBgColor;
	QColor bgcolor;
	bool hasAlpha;
	double alpha;
};

#endif // GAMEOBJECTVIEW_H
