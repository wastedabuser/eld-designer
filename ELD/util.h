#ifndef UTIL_H
#define UTIL_H

#include <QString>

class Util {
  public:
    static void warn(const QString& value);
    static void warn(const char* value1, const QString& value2);
};

#endif // UTIL_H
