#include <QJsonObject>
#include <qjsondocument.h>

#ifndef JSONIO_H
#define JSONIO_H

class JsonIO {
  public:
    static QJsonDocument readJson(const QString& fileName);
    static void writeJson(const QString& fileName, const QJsonDocument& jsonDoc);
};

#endif // JSONIO_H
