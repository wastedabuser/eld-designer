#include "config.h"
#include "gameobjectview.h"
#include "hoverpoints.h"
#include "propertymodel.h"

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
	zoomFactor = scaleF;
	selected = false;
	hasRotation = gameObject->hasProperty("rotation");
	hasBgColor = gameObject->hasProperty("bgColor");
	hasAlpha = gameObject->hasProperty("alpha");
	hasParallax = gameObject->hasProperty("parallax");
	hasPosition = gameObject->hasProperty("x") && gameObject->hasProperty("y");
	hasSize = gameObject->hasProperty("width") || gameObject->hasProperty("height");
	alpha = 1;
	parallax = 1;

	fetchSizeProperties();
	fetchTextureProperty();

	hoverPoints = new HoverPoints(widget, HoverPoints::CircleShape);
	hoverPoints->setShapeBrush(QBrush(defaultFillColor));
	hoverPoints->setShapePen(QPen(defaultColor));
	hoverPoints->setConnectionPen(QPen(defaultFillColor, 0, Qt::DotLine, Qt::FlatCap, Qt::BevelJoin));

	controlPolygon << QPointF(x, y);
	if (hasRotation) controlPolygon << QPointF(x + 100 * zoomFactor, y);

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
	if (hasRotation) p.rotate(rotation);
	p.scale(zoomFactor, zoomFactor);
	p.translate(-center);

	if (!rectangle.isNull()) {
		p.setPen(QPen(selected ? selectedColor : defaultColor, 1 / zoomFactor, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
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
	propagateViewChange();
}

void GameObjectView::onPointsChanged(const QPolygonF &points) {
	QPointF trans = points.at(0) - controlPolygon.at(0);

	if (!trans.isNull()) {
		controlPolygon[0] = points.at(0);
		if (controlPolygon.size() > 1) controlPolygon[1] += trans;
		hoverPoints->setPoints(controlPolygon);

		for (int i = 0; i < childViews.size(); i++) {
			childViews[i]->setDelta(trans.x(), trans.y());
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
	fetchViewShape();
	commitPositionProperties(true);
}

void GameObjectView::cacheRelatedViews() {
	QList<GameObject *> list = gameObject->getChildrenListDeep();
	childViews = container->getViewsForObejcts(list);

	list = gameObject->getParentsList();
	parentViews = container->getViewsForObejcts(list);
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
//	sf *= paralax;
	zoomFactor *= sf;
	QTransform trans;
	trans = trans.scale(sf, sf);
	controlPolygon = trans.map(controlPolygon);
	hoverPoints->setZoomChange(sf);
	if (polyline) polyline->setZoomChange(sf);
}

void GameObjectView::setDelta(int dx, int dy) {
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
		int dy = (container->padding + value.toInt()) * zoomFactor - controlPolygon[0].y();
		setDelta(0, dy);
		cacheRelatedViews();
		for (int i = 0; i < childViews.size(); i++) {
			childViews[i]->setDelta(0, dy);
		}
	} else if (name == "x") {
		int dx = (container->padding + value.toInt()) * zoomFactor - controlPolygon[0].x();
		setDelta(dx, 0);
		cacheRelatedViews();
		for (int i = 0; i < childViews.size(); i++) {
			childViews[i]->setDelta(dx, 0);
		}
	} else if (name == "width") {
		width = value.toInt();
		fetchShapeProperty();
		fetchTextureProperty();
	} else if (name == "height") {
		height = value.toInt();
		fetchShapeProperty();
		fetchTextureProperty();
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
	fetchViewShape();
}

void GameObjectView::fetchViewShape() {
	fetchShapeProperty();
	for (int i = 0; i < parentViews.size(); i++) {
		parentViews[i]->propagateViewChange();
	}
	emit viewChanged(this);
}

QRect GameObjectView::bouds() {
	if (polyline) return polyline->getBounds();
	return hoverPoints->getBounds();
}

void GameObjectView::propagateViewChange() {
	cacheRelatedViews();
	QString shape = gameObject->getPropertyValue("shape");
	if (!shape.isEmpty()) {
		QList<GameObjectView *> meAndChilds;
		if (controlPolygon.size() == 1 && !polyline) {
			meAndChilds = childViews;
		} else {
			meAndChilds.append(childViews);
			meAndChilds.append(this);
		}

		QRect r;
		if (meAndChilds.size() > 0)	r = container->getViewsBounds(meAndChilds);
		else if (shape == "bounds") r.setCoords(controlPolygon[0].x(), controlPolygon[0].y(), controlPolygon[0].x(), controlPolygon[0].y());

		if (!hasPosition) {
			controlPolygon[0].setX(r.x() + r.width() / 2);
			controlPolygon[0].setY(r.y() + r.height() / 2);
			hoverPoints->setPoints(controlPolygon);
		}
		if (shape == "bounds") {
			double x = (r.x() - controlPolygon[0].x()) / zoomFactor;
			double y = (r.y() - controlPolygon[0].y()) / zoomFactor;
			double w = r.width() / zoomFactor;
			double h = r.height() / zoomFactor;
			rectangle = QRectF(x, y, w, h);
		}
	}
}

void GameObjectView::fetchPositionProperties() {
	if (hasPosition) {
		int dx = (container->padding + gameObject->getPropertyValue("x").toInt()) * zoomFactor - controlPolygon[0].x();
		int dy = (container->padding + gameObject->getPropertyValue("y").toInt()) * zoomFactor - controlPolygon[0].y();
		setDelta(dx, dy);
	}
	if (hasRotation) setRotation(gameObject->getPropertyValue("rotation").toDouble());
}

void GameObjectView::fetchSizeProperties() {
	width = gameObject->getPropertyValue("width").toInt();
	height = gameObject->getPropertyValue("height").toInt();
}

void GameObjectView::fetchShapeProperty() {
	QString shape = gameObject->getPropertyValue("shape");
	if (shape == "bounds") {
		// do nothing in this case
		// this functionality depends on the relatedViews
	} else if (shape == "polyline") {
		if (!polyline) {
			polyline = new HoverPoints(container, HoverPoints::RectangleShape, zoomFactor);
			polyline->fromJsonString(gameObject->getPropertyValue("lineCoordinates"), container->padding, container->padding);
			polyline->setConnectionPen(QPen(QColor(gameObject->getPropertyValue("lineColor")), 2));
			polyline->init();
			connect(polyline, SIGNAL(pointsChangeComplete()), this, SLOT(onPolylineChangeComplete()));
		}
	} else if (shape == "rectangle" || image.isNull()) {
		rectangle = QRectF(0, 0, width, height);
	}
	if (hasBgColor) bgColor = QColor(gameObject->getPropertyValue("bgColor"));
}

void GameObjectView::onPolylineChangeComplete() {
	gameObject->setPropertyValue("lineCoordinates", polyline->toJsonString(container->canvasPadding, container->canvasPadding));
	propagateViewChange();
	for (int i = 0; i < parentViews.size(); i++) {
		parentViews[i]->propagateViewChange();
	}
	emit viewChanged(this);
}

void GameObjectView::fetchTextureProperty() {
	if (hasAlpha) alpha = gameObject->getPropertyValue("alpha").toDouble();
	if (hasParallax) parallax = gameObject->getPropertyValue("parallax").toDouble();
	QString path = Config::getResourceAbsolutePath(gameObject->getPropertyValue("texture"));

	if (!path.isEmpty()) {
		image = QImage(path);
		if (!image.isNull()) {
			if (hasSize) {
				if (!width) gameObject->setPropertyValue("width", QString::number(width = image.width()));
				if (!height) gameObject->setPropertyValue("height", QString::number(height = image.height()));
				image = image.scaled(width, height);
			} else {
				width = image.width();
				height = image.height();
			}
		}
		rectangle = QRectF(0, 0, width, height).adjusted(-2, -2, 2, 2);
	}
}

void GameObjectView::commitPositionProperties(bool deep) {
	if (hasPosition) {
		gameObject->setPropertyValue("x", QString::number(qRound((controlPolygon[0].x() - container->canvasPadding) / zoomFactor)));
		gameObject->setPropertyValue("y", QString::number(qRound((controlPolygon[0].y() - container->canvasPadding) / zoomFactor)));
	}
	if (hasRotation) gameObject->setPropertyValue("rotation", QString::number(rotation));
	if (polyline) gameObject->setPropertyValue("lineCoordinates", polyline->toJsonString(container->canvasPadding, container->canvasPadding));

	if (deep) {
		for (int i = 0; i < childViews.size(); i++) {
			childViews[i]->commitPositionProperties(deep);
		}
	}
}
