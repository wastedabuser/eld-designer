#include "gameobjectcontainer.h"
#include <gameobjectview.h>

GameObjectContainer::GameObjectContainer(QWidget *parent) :
	QWidget(parent) {

	setAttribute(Qt::WA_MouseTracking);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setMaximumWidth(1000);
	setMaximumHeight(1000);
}

GameObjectContainer::~GameObjectContainer() {

}

void GameObjectContainer::paintEvent(QPaintEvent *) {
	for (int i = 0; i < views.size(); i++) {
		views[i]->drawPixmap();
	}
}

void GameObjectContainer::addGameObject(GameObject *obj) {
	GameObjectView *view = new GameObjectView(this, obj);

	views.append(view);

	update();
}


