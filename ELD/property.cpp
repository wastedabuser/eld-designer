#include "property.h"

#include <QAbstractItemModel>

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
    if (column == 0)
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEditable;
}

bool Property::setData(int column, const QVariant &val) {
	if (column == 0) name = QVariant(val).toString();
	else value = QVariant(val).toString();
}
