#ifndef HOVERPOINTS_H
#define HOVERPOINTS_H

#include "util.h"

#include <QtGui>
#include <QWidget>

class HoverPoints : public QObject {
	Q_OBJECT

  public:
	enum PointShape {
		CircleShape,
		RectangleShape
	};

	enum SortType {
		NoSort,
		XSort,
		YSort
	};

	enum ConnectionType {
		NoConnection,
		LineConnection,
		CurveConnection
	};

	HoverPoints(QWidget *widget, PointShape shape);

	bool eventFilter(QObject *object, QEvent *event);

	void renderPoints();

	QPolygonF points() const {
		return m_points;
	}
	void setPoints(const QPolygonF &points);

	QSizeF pointSize() const {
		return m_pointSize;
	}
	void setPointSize(const QSizeF &size) {
		m_pointSize = size;
	}
	SortType sortType() const {
		return m_sortType;
	}
	void setSortType(SortType sortType) {
		m_sortType = sortType;
	}
	ConnectionType connectionType() const {
		return m_connectionType;
	}
	void setConnectionType(ConnectionType connectionType) {
		m_connectionType = connectionType;
	}
	void setConnectionPen(const QPen &pen) {
		m_connectionPen = pen;
	}
	void setShapePen(const QPen &pen) {
		m_pointPen = pen;
	}
	void setShapeBrush(const QBrush &brush) {
		m_pointBrush = brush;
	}
	void setEditable(bool editable) {
		m_editable = editable;
	}
	bool editable() const {
		return m_editable;
	}

	void firePointChange();
	QRect getBounds();

  public slots:
	void setEnabled(bool enabled);
	void setDisabled(bool disabled) {
		setEnabled(!disabled);
	}

  signals:
	void pointsChanged(const QPolygonF &points);
	void pointsChangeComplete();

  public:
	void setZoomChange(double sf);

  private:
	QRectF pointBoundingRect(int i) const;
	void movePoint(int i, const QPointF &newPos, bool emitChange = true);

	QWidget *m_widget;

	QPolygonF m_points;
	PointShape m_shape;
	SortType m_sortType;
	ConnectionType m_connectionType;

	QVector<uint> m_locks;

	QSizeF m_pointSize;
	int m_currentIndex;
	bool m_editable;
	bool m_enabled;

	QPen m_pointPen;
	QBrush m_pointBrush;
	QPen m_connectionPen;
};

#endif // HOVERPOINTS_H
