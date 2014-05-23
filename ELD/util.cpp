#include "util.h"

#include <QString>

void Util::warn(const QString& value) {
    qWarning(value.toUtf8().constData());
}

void Util::warn(const char* value1, const QString& value2) {
    Util::warn(value1 + value2);
}
