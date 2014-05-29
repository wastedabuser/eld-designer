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

void GameObjectContainer::addGameObject(GameObject *obj) {
	GameObjectView *view = new GameObjectView(this, obj);

	views.append(view);
	hViews[obj] = view;

	connect(view, SIGNAL(viewChanged(GameObjectView*)),
			this, SLOT(handleViewChange(GameObjectView *)));

	update();
	handleViewChange(view);
}

void GameObjectContainer::removeGameObject(GameObject *obj) {
	GameObjectView *view = hViews[obj];

	views.removeOne(view);
	hViews.remove(obj);

	disconnect(view, SIGNAL(viewChanged(GameObjectView*)),
			   this, SLOT(handleViewChange(GameObjectView *)));

	update();
	handleViewChange(view);

	delete view;
}

void GameObjectContainer::selectGameObject(GameObject *obj) {
	for (int i = 0; i < views.size(); i++) {
		views[i]->selected = false;
	}
	if (hViews.contains(obj)) hViews[obj]->selected = true;

	update();
}

void GameObjectContainer::handleViewChange(GameObjectView *view) {
	view->commitProperties();
	alignContainerCanvas();
	update();
}

void GameObjectContainer::alignContainerCanvas() {
	int minx = 1000000;
	int maxx = -1000000;
	int miny = 1000000;
	int maxy = -1000000;

	for (int i = 0; i < views.size(); i++) {
		QRect bnds = views[i]->bouds();
		if (bnds.left() < minx) minx = bnds.left();
		if (bnds.right() > maxx) maxx = bnds.right();
		if (bnds.top() < miny) miny = bnds.top();
		if (bnds.bottom() > maxy) maxy = bnds.bottom();
	}

	minx -= canvasPadding;
	maxx += canvasPadding;
	miny -= canvasPadding;
	maxy += canvasPadding;

	int w = maxx - minx;
	int h = maxy - miny;

//	Util::warn(" min x:" + QString::number(minx) +
//			   " min y:" + QString::number(miny) +
//			   " max x:" + QString::number(maxx) +
//			   " max y:" + QString::number(maxy)
//			   );

	int mw = minx < 0 ? w : maxx;
	int mh = miny < 0 ? h : maxy;
	dmw = mw - minimumWidth();
	dmh = mh - minimumHeight();
	setMinimumWidth(mw);
	setMinimumHeight(mh);

	if (minx < 0 || miny < 0) {
		for (int i = 0; i < views.size(); i++) {
			views[i]->setDelta(minx < 0 ? -minx : 0, miny < 0 ? -miny : 0);
			views[i]->commitProperties();
		}
	}
}
