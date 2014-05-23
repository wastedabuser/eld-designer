#include "jsonio.h"
#include "util.h"

#include <QFile>
#include <QJsonDocument>

QJsonDocument JsonIO::readJson(const QString& fileName) {
    QString val;
    QFile file;
    QJsonDocument doc;

    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString msg = "Can't open file for read: ";
        msg.append(fileName);
        Util::warn(msg);
        return doc;
    }

    val = file.readAll();
    file.close();

    doc = QJsonDocument::fromJson(val.toUtf8());
    return doc;
}

void JsonIO::writeJson(const QString& fileName, const QJsonDocument& jsonDoc) {
    QFile file;
    file.setFileName(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QString msg = "Can't open file for save: ";
        msg.append(fileName);
        Util::warn(msg);
        return;
    }

    file.write(jsonDoc.toJson());

    file.close();
}
