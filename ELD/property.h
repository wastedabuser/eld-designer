#ifndef PROPERTY_H
#define PROPERTY_H

#include <QVariant>
#include <QJsonObject>

class Property {
  public:
    Property(QString &nm, QString &val, QJsonObject &opt);
    ~Property();

    QVariant data(int column) const;
    Qt::ItemFlags flags(int column) const;
    bool setData(int column, const QVariant &val);

	QString name;
	QString value;

  private:
    QJsonObject options;
};

#endif // PROPERTY_H
