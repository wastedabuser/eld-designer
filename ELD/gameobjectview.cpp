#include "gameobjectview.h"
#include "hoverpoints.h"

#include <QLayout>
#include <QPainter>
#include <QPainterPath>

const int defaultAlpha255 = 155;
const QColor defaultColor = QColor(255, 0, 0, defaultAlpha255);
const QColor selectedColor = QColor(0, 255, 0, defaultAlpha255);
const QColor defaultFillColor = QColor(151, 0, 0, defaultAlpha255);
const QColor selectedFillColor = QColor(0, 151, 0, defaultAlpha255);

GameObjectView::GameObjectView(GameObjectContainer *widget, GameObject *obj, double scaleF, int x, int y) : QObject(widget) {

	container = widget;
	gameObject = obj;
	rotation = 0;
	polyline = 0;
	scaleFactor = scaleF;
	selected = false;
	canRotate = gameObject->hasProperty("rotation");
	hasBgColor = gameObject->hasProperty("bgColor");
	hasAlpha = gameObject->hasProperty("alpha");
	alpha = 1;

	fetchSizeProperties();
	fetchTextureProperty();

	hoverPoints = new HoverPoints(widget, HoverPoints::CircleShape);
	hoverPoints->setShapeBrush(QBrush(defaultFillColor));
	hoverPoints->setShapePen(QPen(defaultColor));
	hoverPoints->setConnectionPen(QPen(defaultFillColor, 0, Qt::DotLine, Qt::FlatCap, Qt::BevelJoin));

	controlPolygon << QPointF(x, y);
	if (canRotate) controlPolygon << QPointF((x + 100) * scaleFactor, y);

	fetchPositionProperties();
	fetchShapeProperty();

	hoverPoints->init();

	connect(hoverPoints, SIGNAL(pointsChangeStart()),
			this, SLOT(onPointsChangeStart()));

	connect(hoverPoints, SIGNAL(pointsChanged(const QPolygonF&)),
			this, SLOT(onPointsChanged(const QPolygonF &)));

	connect(hoverPoints, SIGNAL(pointsChangeComplete()),
			this, SLOT(onPointsChangeComplete()));

	connect(gameObject->propertyModel, SIGNAL(propertyChanged(const QString &, const QString &)),
			this, SLOT(onPropertyModelUpdated(const QString &, const QString &)));

}

GameObjectView::~GameObjectView() {
	delete hoverPoints;
	if(polyline) delete polyline;
}

void GameObjectView::renderView() {
	QPainter p(container);

	p.setRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::SmoothPixmapTransform);

	QPointF center(width/2.0, height/2.0);
	p.translate(controlPolygon.at(0) - center);

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
			p.fillRect(rectangle, bgColor);
		} else {
			p.setBrush(Qt::NoBrush);
		}
	}
	if (!image.isNull()) p.drawImage(QPointF(0, 0), image);
}

void GameObjectView::onPointsChangeStart() {
	createRelatedViewsBoundingRectangle();
}

void GameObjectView::onPointsChanged(const QPolygonF &points) {
	QPointF trans = points.at(0) - controlPolygon.at(0);

	if (points.size() > 1 && qAbs(points.at(0).x() - points.at(1).x()) < 10 && qAbs(points.at(0).y() - points.at(1).y()) < 10)
		hoverPoints->setPoints(controlPolygon);

	if (!trans.isNull()) {
		controlPolygon[0] = points.at(0);
		if (controlPolygon.size() > 1) controlPolygon[1] += trans;
		hoverPoints->setPoints(controlPolygon);

		for (int i = 0; i < relatedViews.size(); i++) {
			relatedViews[i]->setDelta(trans.x(), trans.y());
		}

		if (polyline) polyline->movePolygonDelta(trans);
	}
	controlPolygon = points;

	if (controlPolygon.size() > 1) {
		QLineF line(controlPolygon.at(0), controlPolygon.at(1));
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

void GameObjectView::select(bool state) {
	if (selected == state) return;

	selected = state;
	if (selected) {
		hoverPoints->setShapePen(QPen(selectedColor));
		hoverPoints->setShapeBrush(QBrush(selectedFillColor));
		if (polyline) polyline->setEditable(true);
	} else {
		hoverPoints->setShapePen(QPen(defaultColor));
		hoverPoints->setShapeBrush(QBrush(defaultFillColor));
		if (polyline) polyline->setEditable(false);
	}
}

void GameObjectView::setRotation(qreal r) {
	qreal old_rot = rotation;
	rotation = r;

	QPointF center(controlPolygon.at(0));
	QMatrix m;
	m.translate(center.x(), center.y());
	m.rotate(rotation - old_rot);
	m.translate(-center.x(), -center.y());
	controlPolygon = m.map(controlPolygon);
	hoverPoints->setPoints(controlPolygon);
}

void GameObjectView::setZoomChange(double sf) {
	scaleFactor *= sf;
	QTransform trans;
	trans = trans.scale(sf, sf);
	controlPolygon = trans.map(controlPolygon);
	hoverPoints->setZoomChange(sf);
	if (polyline) polyline->setZoomChange(sf);
}

void GameObjectView::setDelta(int dx, int dy) {
//	if (dx == 0 && dy == 0) return;

	for (int i = 0; i < controlPolygon.size(); i++) {
		controlPolygon[i].setX(controlPolygon[i].x() + dx);
		controlPolygon[i].setY(controlPolygon[i].y() + dy);
	}
	hoverPoints->setPoints(controlPolygon);
	if (polyline) polyline->moveDelta(dx, dy);
}

void GameObjectView::onPropertyModelUpdated(const QString &name, const QString &value) {
	if (name == "texture") {
		fetchTextureProperty();
	} else if (name == "y") {
		int dy = value.toInt() * scaleFactor - controlPolygon[0].y();
		setDelta(0, dy);
		cacheRelatedViews();
		for (int i = 0; i < relatedViews.size(); i++) {
			relatedViews[i]->setDelta(0, dy);
		}
	} else if (name == "x") {
		int dx = value.toInt() * scaleFactor - controlPolygon[0].x();
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
	} else if (name == "bgColor") {
		bgColor = QColor(value);
	} else if (name == "lineColor") {
		if (polyline) polyline->setConnectionPen(QPen(QColor(value), 2));
	} else if (name == "alpha") {
		alpha = value.toDouble();
	} else {
		return;
	}
	onPointsChangeComplete();
}

QRect GameObjectView::bouds() {
	return hoverPoints->getBounds();
}

void GameObjectView::createRelatedViewsBoundingRectangle() {
	cacheRelatedViews();
	QString shape = gameObject->getPropertyValue("shape");
	if (shape == "bounds") {
		QRect r = container->getViewsBounds(relatedViews);
		rectangle = QRectF((r.x() - controlPolygon[0].x()) / scaleFactor, (r.y() - controlPolygon[0].y()) / scaleFactor, r.width() / scaleFactor, r.height() / scaleFactor);
	}
}

void GameObjectView::fetchPositionProperties() {
	int dx = gameObject->getPropertyValue("x").toInt() * scaleFactor - controlPolygon[0].x();
	int dy = gameObject->getPropertyValue("y").toInt() * scaleFactor - controlPolygon[0].y();
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
		// do nothing in this case
	} else if (shape == "polyline") {
		if (!polyline) {
			polyline = new HoverPoints(container, HoverPoints::RectangleShape, scaleFactor);
			polyline->fromJsonString(gameObject->getPropertyValue("lineCoordinates"));
			polyline->setConnectionPen(QPen(QColor(gameObject->getPropertyValue("lineColor")), 2));
			polyline->init();
			connect(polyline, SIGNAL(pointsChangeComplete()), this, SLOT(onPolylineChangeComplete()));
		}
	} else if (shape == "rectangle" || image.isNull()) {
		rectangle = QRectF(0, 0, width, height);
	}
	if (hasBgColor) bgColor = QColor(gameObject->getPropertyValue("bgColor"));
}

void GameObjectView::onPolylineChangeComplete(){
	gameObject->setPropertyValue("lineCoordinates", polyline->toJsonStrign());
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
	gameObject->setPropertyValue("x", QString::number(qRound(controlPolygon[0].x() / scaleFactor)));
	gameObject->setPropertyValue("y", QString::number(qRound(controlPolygon[0].y() / scaleFactor)));
	if (canRotate) gameObject->setPropertyValue("rotation", QString::number(rotation));
}
