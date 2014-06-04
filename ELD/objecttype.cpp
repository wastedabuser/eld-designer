#include "objecttype.h"

ObjectType::ObjectType(QJsonObject &opt) {
	type = opt["type"].toString();
	definition = opt;
}
