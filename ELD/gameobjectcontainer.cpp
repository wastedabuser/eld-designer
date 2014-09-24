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

	padding = 5000;
	canvasPadding = padding;
	scaleFactor = 1;
}

GameObjectContainer::~GameObjectContainer() {
	delete ui;
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

void GameObjectContainer::centerView() {
	QScrollArea *scrollArea = (QScrollArea *) parent()->parent();
	scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->maximum() / 2);
	scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum() / 2);
}

void GameObjectContainer::zoom(bool out, bool wheel) {
	if (views.empty()) return;

	double sf;
	if (out) {
		sf = zoomStep;
	} else {
		sf = 1 / zoomStep;
	}

	for (int i = 0; i < views.size(); i++) {
		views[i]->setZoomChange(sf);
	}

	scaleFactor *= sf;
	canvasPadding *= sf;

	alignContainerCanvas();
	update();

	QScrollArea *scrollArea = (QScrollArea *) parent()->parent();
	QPoint pt;
	if (wheel) {
		pt = mapFromGlobal(QCursor::pos());
	} else {
		pt = mapFromParent(QPoint(scrollArea->width() / 2, scrollArea->height() / 2));
	}
	double pivotX = (double)pt.x() * sf;
	double pivotY = (double)pt.y() * sf;
	scrollArea->horizontalScrollBar()->setValue((double)scrollArea->horizontalScrollBar()->value() + pivotX - pt.x());
	scrollArea->verticalScrollBar()->setValue((double)scrollArea->verticalScrollBar()->value() + pivotY - pt.y());
}

void GameObjectContainer::addGameObject(GameObject *obj, bool doUpdate) {
	if (!obj || obj == NULL) return;

	QList<GameObject *> list = obj->getChildrenListDeep();
	list.append(obj);

	for (int i = list.size() - 1; i >= 0; i--) {
		if (!list[i]->hasView()) continue;

		GameObjectView *view = new GameObjectView(this, list[i], scaleFactor, canvasPadding, canvasPadding);

		views.append(view);
		hViews[list[i]] = view;

		connect(view, SIGNAL(viewChanged(GameObjectView*)),
				this, SLOT(handleViewChange(GameObjectView *)));
	}

	for (int i = 0; i < list.size(); i++) {
		GameObject *obj = list[i];
		if (!hViews.contains(obj)) continue;
		hViews[obj]->propagateViewChange();
	}

	if (doUpdate) updateCanvas();
}

void GameObjectContainer::removeGameObject(GameObject *obj) {
	if (!obj || obj == NULL) return;

	QList<GameObject *> list = obj->getChildrenListDeep();
	list.append(obj);

	for (int i = 0; i < list.size(); i++) {
		if (!hViews.contains(list[i])) continue;
		GameObjectView *view = hViews[list[i]];

		views.removeOne(view);
		hViews.remove(list[i]);

		disconnect(view, SIGNAL(viewChanged(GameObjectView*)),
				   this, SLOT(handleViewChange(GameObjectView *)));

		delete view;
	}

	updateCanvas();
}

void GameObjectContainer::applyGameObjectsOrder(QList<GameObject *> &list) {
	for (int i = list.size() - 1; i >= 0; i--) {
		if (!hViews.contains(list[i])) continue;
		GameObjectView *view = hViews[list[i]];
		if (views.removeOne(view)) views.append(view);
	}
	update();
}

void GameObjectContainer::updateCanvas() {
	alignContainerCanvas();
	update();
}

void GameObjectContainer::selectGameObject(GameObject *obj) {
	for (int i = 0; i < views.size(); i++) {
		views[i]->select(false);
	}
	if (hViews.contains(obj)) hViews[obj]->select(true);

	update();
}

void GameObjectContainer::selectView(GameObjectView *view) {
	for (int i = 0; i < views.size(); i++) {
		if (view == views[i]) {
			views[i]->select(true);
			emit gameObjectSelect(view->gameObject);
		} else {
			views[i]->select(false);
		}
	}
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
	selectView(view);
	updateCanvas();
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
			views[i]->commitPositionProperties(false);
		}
	}
}
