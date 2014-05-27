#include "gameobjectcontainer.h"
#include "ui_gameobjectcontainer.h"

#include <QScrollArea>
#include <gameobjectview.h>

GameObjectContainer::GameObjectContainer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GameObjectContainer) {
	ui->setupUi(this);
}

GameObjectContainer::~GameObjectContainer() {
	delete ui;
}

void GameObjectContainer::paintEvent(QPaintEvent *) {
	for (int i = 0; i < views.size(); i++) {
		views[i]->drawPixmap();
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
	GameObjectView *view = getView(obj);

	views.removeOne(view);
	hViews.remove(obj);

	disconnect(view, SIGNAL(viewChanged(GameObjectView*)),
			this, SLOT(handleViewChange(GameObjectView *)));

	delete view;

	update();
	handleViewChange();
}

GameObjectView *GameObjectContainer::getView(GameObject *obj) {
	return hViews[obj];
}

void GameObjectContainer::handleViewChange(GameObjectView *view) {
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

	int pw = parentWidget()->width();
	int ph = parentWidget()->height();
	minx -= pw / 2;
	maxx += pw / 2;
	miny -= ph / 2;
	maxy += ph / 2;

	int w = maxx - minx;
	int h = maxy - miny;
	/*
	Util::warn(" min x:" + QString::number(minx) +
			   " min y:" + QString::number(miny) +
			   " max x:" + QString::number(maxx) +
			   " max y:" + QString::number(maxy)
			   );
	*/
	setMinimumWidth(w);
	setMinimumHeight(h);
	for (int i = 0; i < views.size(); i++) {
		views[i]->applyDelta(-minx, -miny);
		views[i]->commitProperties();
	}
}
