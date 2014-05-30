#include "gameobjectview.h"
#include "hoverpoints.h"

#include <QLayout>
#include <QPainter>
#include <QPainterPath>

const int defaultAlpha255 = 155;
const QColor defaultColor = QColor(255, 0, 0, defaultAlpha255);
const QColor selectedColor = QColor(0, 255, 0, defaultAlpha255);

GameObjectView::GameObjectView(GameObjectContainer *widget, GameObject *obj, double scaleF) : QObject(widget) {

	container = widget;
	gameObject = obj;
	rotation = 0;
	scaleFactor = scaleF;
	selected = false;
	canRotate = gameObject->hasProperty("rotation");
	hasBgColor = gameObject->hasProperty("bgcolor");
	hasAlpha = gameObject->hasProperty("alpha");
	alpha = 1;

	fetchSizeProperties();
	fetchTextureProperty();

	pts = new HoverPoints(widget, HoverPoints::CircleShape);
	pts->setConnectionType(HoverPoints::LineConnection);
	pts->setEditable(false);
	pts->setPointSize(QSize(10, 10));
	pts->setShapeBrush(QBrush(QColor(151, 0, 0, defaultAlpha255)));
	pts->setShapePen(QPen(QColor(255, 100, 50, defaultAlpha255)));
	pts->setConnectionPen(QPen(QColor(151, 0, 0, defaultAlpha255), 0, Qt::DotLine, Qt::FlatCap, Qt::BevelJoin));

	int pw = width / 2;
	int ph = height / 2;
	ctrlPoints << QPointF(pw, ph);
	if (canRotate) ctrlPoints << QPointF(pw + 100 * scaleFactor, ph);

	fetchShapeProperty();
	fetchPositionProperties();

	connect(pts, SIGNAL(pointsChangeStart()),
			this, SLOT(onPointsChangeStart()));

	connect(pts, SIGNAL(pointsChanged(const QPolygonF&)),
			this, SLOT(onPointsChanged(const QPolygonF &)));

	connect(pts, SIGNAL(pointsChangeComplete()),
			this, SLOT(onPointsChangeComplete()));

	connect(gameObject->propertyModel, SIGNAL(propertyChanged(const QString &, const QString &)),
			this, SLOT(onPropertyModelUpdated(const QString &, const QString &)));

}

GameObjectView::~GameObjectView() {
	delete pts;
}

void GameObjectView::renderView() {
	QPainter p(container);

	p.setRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::SmoothPixmapTransform);

	QPointF center(width/2.0, height/2.0);
	p.translate(ctrlPoints.at(0) - center);

	p.translate(center);
	if (canRotate) p.rotate(rotation);
	p.scale(scaleFactor, scaleFactor);
	p.translate(-center);

	if (!rectangle.isNull()) {
		p.setPen(QPen(selected ? selectedColor : defaultColor, 1 / scaleFactor, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
		p.drawRect(rectangle);
	}

	if (hasAlpha) p.setOpacity(alpha);
	if (!rectangle.isNull()) {
		if (hasBgColor) {
			p.setBrush(Qt::SolidPattern);
			p.fillRect(rectangle, bgcolor);
		} else {
			p.setBrush(Qt::NoBrush);
		}
	}
	if (!image.isNull()) p.drawImage(QPointF(0, 0), image);
}

void GameObjectView::onPointsChangeStart() {
	cacheRelatedViews();
}

void GameObjectView::onPointsChanged(const QPolygonF &points) {
	QPointF trans = points.at(0) - ctrlPoints.at(0);

	if (points.size() > 1 && qAbs(points.at(0).x() - points.at(1).x()) < 10 && qAbs(points.at(0).y() - points.at(1).y()) < 10)
		pts->setPoints(ctrlPoints);

	if (!trans.isNull()) {
		ctrlPoints[0] = points.at(0);
		if (ctrlPoints.size() > 1) ctrlPoints[1] += trans;
		pts->setPoints(ctrlPoints);

		for (int i = 0; i < relatedViews.size(); i++) {
			relatedViews[i]->setDelta(trans.x(), trans.y());
		}

	}
	ctrlPoints = points;

	if (ctrlPoints.size() > 1) {
		QLineF line(ctrlPoints.at(0), ctrlPoints.at(1));
		rotation = line.angle(QLineF(0, 0, 1, 0));
		if (line.dy() < 0)
			rotation = 360 - rotation;
	}
}

void GameObjectView::onPointsChangeComplete() {
	fetchShapeProperty();
	emit viewChanged(this);
}

void GameObjectView::cacheRelatedViews() {
	QList<GameObject *> list = gameObject->getChildrenListDeep();
	relatedViews = container->getViewsForObejcts(list);
}

void GameObjectView::setRotation(qreal r) {
	qreal old_rot = rotation;
	rotation = r;

	QPointF center(ctrlPoints.at(0));
	QMatrix m;
	m.translate(center.x(), center.y());
	m.rotate(rotation - old_rot);
	m.translate(-center.x(), -center.y());
	ctrlPoints = m.map(ctrlPoints);
	pts->setPoints(ctrlPoints);
}

void GameObjectView::setZoomChange(double sf) {
	scaleFactor *= sf;
	QTransform trans;
	trans = trans.scale(sf, sf);
	ctrlPoints = trans.map(ctrlPoints);
	pts->setZoomChange(sf);
}

void GameObjectView::setDelta(int dx, int dy) {
	if (dx == 0 && dy == 0) return;

	for (int i = 0; i < ctrlPoints.size(); i++) {
		ctrlPoints[i].setX(ctrlPoints[i].x() + dx);
		ctrlPoints[i].setY(ctrlPoints[i].y() + dy);
	}
	pts->setPoints(ctrlPoints);
}

void GameObjectView::onPropertyModelUpdated(const QString &name, const QString &value) {
	if (name == "texture") {
		fetchTextureProperty();
	} else if (name == "y") {
		int dy = value.toInt() * scaleFactor - ctrlPoints[0].y();
		setDelta(0, dy);
		cacheRelatedViews();
		for (int i = 0; i < relatedViews.size(); i++) {
			relatedViews[i]->setDelta(0, dy);
		}
	} else if (name == "x") {
		int dx = value.toInt() * scaleFactor - ctrlPoints[0].x();
		setDelta(dx, 0);
		cacheRelatedViews();
		for (int i = 0; i < relatedViews.size(); i++) {
			relatedViews[i]->setDelta(dx, 0);
		}
	} else if (name == "width") {
		width = value.toInt();
		fetchShapeProperty();
	} else if (name == "height") {
		height = value.toInt();
		fetchShapeProperty();
	} else if (name == "rotation") {
		setRotation(value.toFloat());
	} else if (name == "bgcolor") {
		bgcolor = QColor(value);
	} else if (name == "alpha") {
		alpha = value.toDouble();
	} else {
		return;
	}
	onPointsChangeComplete();
}

QRect GameObjectView::bouds() {
	return pts->getBounds();
}

void GameObjectView::fetchPositionProperties() {
	int dx = gameObject->getPropertyValue("x").toInt() * scaleFactor - ctrlPoints[0].x();
	int dy = gameObject->getPropertyValue("y").toInt() * scaleFactor - ctrlPoints[0].y();
	setDelta(dx, dy);
	if (canRotate) setRotation(gameObject->getPropertyValue("rotation").toDouble());
}

void GameObjectView::fetchSizeProperties() {
	width = gameObject->getPropertyValue("width").toInt();
	height = gameObject->getPropertyValue("height").toInt();
}

void GameObjectView::fetchShapeProperty() {
	QString shape = gameObject->getPropertyValue("shape");
	if (shape == "bounds") {
		QRect r = container->getViewsBounds(relatedViews);
		rectangle = QRectF((r.x() - ctrlPoints[0].x()) / scaleFactor, (r.y() - ctrlPoints[0].y()) / scaleFactor, r.width() / scaleFactor, r.height() / scaleFactor);
	} else if (shape == "rectangle" || image.isNull()) {
		rectangle = QRectF(0, 0, width, height);
	}
	if (hasBgColor) bgcolor = QColor(gameObject->getPropertyValue("bgcolor"));
}

void GameObjectView::fetchTextureProperty() {
	if (hasAlpha) alpha = gameObject->getPropertyValue("alpha").toDouble();
	QString path = gameObject->getPropertyValue("texture");
	if (!path.isEmpty()) {
		image = QImage(path);
		if (!image.isNull()) {
			width = image.width();
			height = image.height();
		}
		rectangle = QRectF(0, 0, width, height).adjusted(-2, -2, 2, 2);
	}
}

void GameObjectView::commitPositionProperties() {
	gameObject->setPropertyValue("x", QString::number(qRound(ctrlPoints[0].x() / scaleFactor)));
	gameObject->setPropertyValue("y", QString::number(qRound(ctrlPoints[0].y() / scaleFactor)));
	if (canRotate) gameObject->setPropertyValue("rotation", QString::number(rotation));
}
