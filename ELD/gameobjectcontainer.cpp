#include "gameobjectcontainer.h"
#include "ui_gameobjectcontainer.h"

#include <QScrollArea>
#include <QScrollBar>
#include <gameobjectview.h>

double zoomStep = 1.15;

GameObjectContainer::GameObjectContainer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GameObjectContainer) {
	ui->setupUi(this);

	canvasPadding = 100;
}

GameObjectContainer::~GameObjectContainer() {
	delete ui;
}

void GameObjectContainer::mouseMoveEvent(QMouseEvent *event) {
	QScrollArea *scrollArea = (QScrollArea *) parent()->parent();
	rx = (double)event->x() / (double)scrollArea->width();
	ry = (double)event->y() / (double)scrollArea->height();
}

void GameObjectContainer::paintEvent(QPaintEvent *) {
	if (views.size() > 1) {
		QPainter p(this);

		p.setRenderHint(QPainter::Antialiasing);
		p.setRenderHint(QPainter::SmoothPixmapTransform);

		p.setPen(QPen(QColor(0,0,0), .25, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
		p.setBrush(Qt::white);

		int paddingX2 = canvasPadding * 2;
		p.drawRect(QRectF(canvasPadding, canvasPadding, minimumWidth() - paddingX2, minimumHeight() - paddingX2));
	}

	for (int i = 0; i < views.size(); i++) {
		views[i]->renderView();
	}
}

void GameObjectContainer::wheelEvent(QWheelEvent *event) {
	zoom(event->delta() > 0, true);
}

void GameObjectContainer::zoom(bool out, bool wheel) {
	double sf;
	if (out) {
		sf = zoomStep;
	} else {
		sf = 1 / zoomStep;
	}

	for (int i = 0; i < views.size(); i++) {
		views[i]->setZoomChange(sf);
	}

	canvasPadding *= sf;
	alignContainerCanvas();

	QScrollArea *scrollArea = (QScrollArea *) parent()->parent();
	scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() + dmw * (wheel ? rx : .5));
	scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->value() + dmh * (wheel ? ry : .5));

	update();
}

void GameObjectContainer::addGameObject(GameObject *obj, bool doUpdate) {
	GameObjectView *view = new GameObjectView(this, obj);

	views.append(view);
	hViews[obj] = view;

	connect(view, SIGNAL(viewChanged(GameObjectView*)),
			this, SLOT(handleViewChange(GameObjectView *)));

	if (doUpdate) handleViewChange(view);
}

void GameObjectContainer::removeGameObject(GameObject *obj) {
	GameObjectView *view = hViews[obj];

	views.removeOne(view);
	hViews.remove(obj);

	disconnect(view, SIGNAL(viewChanged(GameObjectView*)),
			   this, SLOT(handleViewChange(GameObjectView *)));

	handleViewChange(view);

	delete view;
}

void GameObjectContainer::initViews() {
	for (int i = 0; i < views.size(); i++) {
		views[i]->cacheRelatedViews();
		views[i]->fetchShapeProperty();
	}

	alignContainerCanvas();
	update();
}

void GameObjectContainer::selectGameObject(GameObject *obj) {
	for (int i = 0; i < views.size(); i++) {
		views[i]->selected = false;
	}
	if (hViews.contains(obj)) hViews[obj]->selected = true;

	update();
}

QList<GameObjectView *> GameObjectContainer::getViewsForObejcts(QList<GameObject *> &list) {
	QList<GameObjectView *> viewsList;
	for (int i = 0; i < list.size(); i++) {
		if (hViews.contains(list[i])) viewsList.append(hViews[list[i]]);
	}
	return viewsList;
}

QRect GameObjectContainer::getViewsBounds(QList<GameObjectView *> &list) {
	int minx = 1000000;
	int maxx = -1000000;
	int miny = 1000000;
	int maxy = -1000000;

	for (int i = 0; i < list.size(); i++) {
		QRect bnds = list[i]->bouds();
		if (bnds.left() < minx) minx = bnds.left();
		if (bnds.right() > maxx) maxx = bnds.right();
		if (bnds.top() < miny) miny = bnds.top();
		if (bnds.bottom() > maxy) maxy = bnds.bottom();
	}

	return QRect(minx, miny, maxx - minx, maxy - miny);
}

void GameObjectContainer::handleViewChange(GameObjectView *view) {
	view->commitPositionProperties();
	alignContainerCanvas();
	update();
}

void GameObjectContainer::alignContainerCanvas() {
	QRect bounds = getViewsBounds(views).adjusted(-canvasPadding, -canvasPadding, canvasPadding, canvasPadding);

	int minx = bounds.left();
	int maxx = bounds.right();
	int miny = bounds.top();
	int maxy = bounds.bottom();
	int mw = minx < 0 ? bounds.width() : maxx;
	int mh = miny < 0 ? bounds.height() : maxy;

	dmw = mw - minimumWidth();
	dmh = mh - minimumHeight();
	setMinimumWidth(mw);
	setMinimumHeight(mh);

	if (minx < 0 || miny < 0) {
		for (int i = 0; i < views.size(); i++) {
			views[i]->setDelta(minx < 0 ? -minx : 0, miny < 0 ? -miny : 0);
			views[i]->commitPositionProperties();
		}
	}
}
