#include "gameobjectview.h"
#include "hoverpoints.h"

#include <QLayout>
#include <QPainter>
#include <QPainterPath>

const int alpha = 155;
const QColor defaultColor = QColor(255, 0, 0, alpha);
const QColor selectedColor = QColor(0, 255, 0, alpha);

GameObjectView::GameObjectView(GameObjectContainer *widget, GameObject *obj) : QObject(widget) {

	container = widget;
	gameObject = obj;
	rotation = 0;
	scaleFactor = 1;
	selected = false;

	fetchSizeProperties();
	fetchTextureProperty();

	pts = new HoverPoints(widget, HoverPoints::CircleShape);
	pts->setConnectionType(HoverPoints::LineConnection);
	pts->setEditable(false);
	pts->setPointSize(QSize(10, 10));
	pts->setShapeBrush(QBrush(QColor(151, 0, 0, alpha)));
	pts->setShapePen(QPen(QColor(255, 100, 50, alpha)));
	pts->setConnectionPen(QPen(QColor(151, 0, 0, alpha), 0, Qt::DotLine, Qt::FlatCap, Qt::BevelJoin));

	int pw = width / 2;
	int ph = height / 2;
	ctrlPoints << QPointF(pw, ph) << QPointF(pw + 100, ph);

	fetchPositionProperties();

	connect(pts, SIGNAL(pointsChanged(const QPolygonF&)),
			this, SLOT(updateCtrlPoints(const QPolygonF &)));

	connect(pts, SIGNAL(pointsChangeComplete()),
			this, SLOT(updateComplete()));

	connect(gameObject->propertyModel, SIGNAL(propertyChanged(const QString &, const QString &)),
			this, SLOT(propertyModelUpdated(const QString &, const QString &)));

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
	p.rotate(rotation);
	p.scale(scaleFactor, scaleFactor);
	p.translate(-center);

	if (!image.isNull()) p.drawImage(QPointF(0, 0), image);
	p.setPen(QPen(selected ? selectedColor : defaultColor, 1 / scaleFactor, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
	p.setBrush(Qt::NoBrush);
	p.drawRect(QRectF(0, 0, width, height).adjusted(-2, -2, 2, 2));
}

void GameObjectView::updateCtrlPoints(const QPolygonF &points) {
	QPointF trans = points.at(0) - ctrlPoints.at(0);

	if (qAbs(points.at(0).x() - points.at(1).x()) < 10
			&& qAbs(points.at(0).y() - points.at(1).y()) < 10)
		pts->setPoints(ctrlPoints);
	if (!trans.isNull()) {
		ctrlPoints[0] = points.at(0);
		ctrlPoints[1] += trans;
		pts->setPoints(ctrlPoints);
	}
	ctrlPoints = points;

	QLineF line(ctrlPoints.at(0), ctrlPoints.at(1));
	rotation = line.angle(QLineF(0, 0, 1, 0));
	if (line.dy() < 0)
		rotation = 360 - rotation;
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

void GameObjectView::updateComplete() {
	emit viewChanged(this);
}

void GameObjectView::propertyModelUpdated(const QString &name, const QString &value) {
	if (name == "texture") {
		fetchTextureProperty();
	} else if (name == "y") {
		int dy = value.toInt() * scaleFactor - ctrlPoints[0].y();
		setDelta(0, dy);
	} else if (name == "x") {
		int dx = value.toInt() * scaleFactor - ctrlPoints[0].x();
		setDelta(dx, 0);
	} else if (name == "width") {
		width = value.toInt() * scaleFactor;
	} else if (name == "height") {
		height = value.toInt() * scaleFactor;
	} else if (name == "rotation") {
		setRotation(value.toFloat());
	} else {
		return;
	}
	updateComplete();
}

QRect GameObjectView::bouds() {
	return pts->getBounds();
}

void GameObjectView::fetchSizeProperties() {
	width = gameObject->getPropertyValue("width").toInt() * scaleFactor;
	height = gameObject->getPropertyValue("height").toInt() * scaleFactor;
}

void GameObjectView::fetchPositionProperties() {
	int dx = gameObject->getPropertyValue("x").toInt() * scaleFactor - ctrlPoints[0].x();
	int dy = gameObject->getPropertyValue("y").toInt() * scaleFactor - ctrlPoints[0].y();
	setDelta(dx, dy);
	setRotation(gameObject->getPropertyValue("rotation").toDouble());
}

void GameObjectView::fetchTextureProperty() {
	QString path = gameObject->getPropertyValue("texture");
	if (!path.isEmpty()) {
		image = QImage(path);
		if (!image.isNull()) {
			width = image.width() * scaleFactor;
			height = image.height() * scaleFactor;
		}
	}
}

void GameObjectView::commitProperties() {
	gameObject->setPropertyValue("x", QString::number(qRound(ctrlPoints[0].x() / scaleFactor)));
	gameObject->setPropertyValue("y", QString::number(qRound(ctrlPoints[0].y() / scaleFactor)));
	gameObject->setPropertyValue("rotation", QString::number(rotation));
}
