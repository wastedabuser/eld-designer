#include "jsonio.h"
#include "util.h"

#include <QFile>
#include <QJsonDocument>
#include <QStringList>
#include <QJsonArray>

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

QJsonValue JsonIO::findNode(const QJsonDocument &doc, const QString &str) {
	QStringList list = str.split(".");
	QJsonObject o;
	QJsonArray a;
	QJsonValue vl;
	bool tp;
	if (doc.isArray()) {
		a = doc.array();
		tp = false;
	} else {
		o = doc.object();
		tp = true;
	}
	for (int i = 0; i < list.size(); i++) {
		QString ch = list[i];
		int chi = ch.toInt();
		if (tp) {
			if (o[ch].isObject()) {
				o = o[ch].toObject();
				tp = true;
			} else if (o[ch].isArray()) {
				a = o[ch].toArray();
				tp = false;
			} else {
				vl = o[ch];
			}
		} else {
			if (a[chi].isObject()) {
				o = a[chi].toObject();
				tp = true;
			} else if (a[chi].isArray()) {
				a = a[chi].toArray();
				tp = false;
			} else {
				vl = a[chi];
			}
		}
	}
	return vl;
}
