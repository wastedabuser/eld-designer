#include "property.h"

#include <QAbstractItemModel>
#include <QJsonArray>

Property::Property(QString &nm, QString &val, QJsonObject &opt) {
    name = nm;
    value = val;
    options = opt;
}

Property::~Property() {

}

QVariant Property::data(int column) const {
    return column == 0 ? name : value;
}

Qt::ItemFlags Property::flags(int column) const {
	if (column == 0 || readOnly())
		return Qt::ItemIsEnabled;

    return Qt::ItemIsEditable;
}

bool Property::setData(int column, const QVariant &val) {
	QString v = QVariant(val).toString();
	if (column == 0) name = v;
	else {
		if (v != value) value = v;
		else return false;
	}
	return true;
}

bool Property::readOnly() const {
	return name.left(1) == "_" && name.right(1) == "_";
}

QString Property::getType() {
	return options["type"].toString();
}

QString Property::getMeta() {
	return options["meta"].toString();
}

QJsonArray Property::getOptions() {
	return options["options"].toArray();
}
