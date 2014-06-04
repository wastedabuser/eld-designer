#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

#include <QJsonObject>

class ObjectType
{
	public:
		ObjectType(QJsonObject &opt);

		QString type;

	private:
		QJsonObject definition;
};

#endif // OBJECTTYPE_H
