#include "gameobjectview.h"
#include "hoverpoints.h"

#include <QLayout>
#include <QPainter>
#include <QPainterPath>

const int alpha = 155;

GameObjectView::GameObjectView(GameObjectContainer *widget, GameObject *obj) : QObject(widget) {

	container = widget;
	gameObject = obj;
	m_rotation = 0.0;
	m_scale = 1.0;
	m_shear = 0.0;

	fetchProperties();

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
	pts->setPoints(ctrlPoints);

	connect(pts, SIGNAL(pointsChanged(const QPolygonF&)),
			this, SLOT(updateCtrlPoints(const QPolygonF &)));

	connect(pts, SIGNAL(pointsChangeComplete()),
			this, SLOT(updateComplete()));

	connect(gameObject->propertyModel, SIGNAL(propertyChanged(Property *)),
			this, SLOT(propertyModelUpdated(Property *)));

}

GameObjectView::~GameObjectView() {
	delete pts;
}

void GameObjectView::drawPixmap() {
	QPainter p(container);

	p.setRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::SmoothPixmapTransform);

	QPointF center(width/2.0, height/2.0);
	p.translate(ctrlPoints.at(0) - center);

	p.translate(center);
	p.rotate(m_rotation);
	p.scale(m_scale, m_scale);
	p.shear(0, m_shear);
	p.translate(-center);

	if (!pixmap.isNull()) p.drawImage(QPointF(0, 0), pixmap);
	p.setPen(QPen(QColor(255, 0, 0, alpha), 0.25, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
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
	m_rotation = line.angle(QLineF(0, 0, 1, 0));
	if (line.dy() < 0)
		m_rotation = 360 - m_rotation;


}

void GameObjectView::applyDelta(int dx, int dy) {
	for (int i = 0; i < ctrlPoints.size(); i++) {
		ctrlPoints[i].setX(ctrlPoints[i].x() + dx);
		ctrlPoints[i].setY(ctrlPoints[i].y() + dy);
	}
	//Util::warn("x: " + QString::number((int)ctrlPoints[0].x()));
	//Util::warn("y: " + QString::number((int)ctrlPoints[0].y()));
	pts->setPoints(ctrlPoints);
	pts->firePointChange();
}

void GameObjectView::updateComplete() {
	emit viewChanged(this);
}

void GameObjectView::propertyModelUpdated(Property *item) {
	fetchProperties();
}

QRect GameObjectView::bouds() {
	return pts->getBounds();
}

void GameObjectView::fetchProperties() {
	width = gameObject->getPropertyValue("width").toInt();
	height = gameObject->getPropertyValue("height").toInt();
	QString path = gameObject->getPropertyValue("texture");
	if (!path.isEmpty()) {
		pixmap = QImage(path);
		if (!pixmap.isNull()) {
			width = pixmap.width();
			height = pixmap.height();
		}
	}
	updateComplete();
}

void GameObjectView::commitProperties() {
	gameObject->setPropertyValue("x", QString::number((int)ctrlPoints[0].x()));
	gameObject->setPropertyValue("y", QString::number((int)ctrlPoints[0].y()));
	gameObject->setPropertyValue("rotation", QString::number(m_rotation));
}
