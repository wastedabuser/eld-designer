#include "textureatlas.h"
#include "util.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QRegularExpression>

TextureAtlas::TextureAtlas(QString path) {
	parseAtlas(path);
	if (texture.isNull()) texture = QImage(path);
}

void TextureAtlas::parseAtlas(QString path) {
	QFile xmlFile(path);
	if (!xmlFile.exists()) {
		Util::warn("Atlas does not exist: " + path);
		return;
	}
	if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		Util::warn("Atlas can not be open: " + path);
		return;
	}
	QXmlStreamReader xml(&xmlFile);
	while (!xml.atEnd()) {
		if (xml.readNext() != QXmlStreamReader::StartElement) continue;
		if (xml.name().toString() == "TextureAtlas") {
			QXmlStreamAttributes attr = xml.attributes();
			QString textureFileName = attr.value("imagePath").toString();
			QRegularExpression re("[^\\/]+$");
			QString texturePath = path.replace(re, textureFileName);
			texture = QImage(texturePath);
		}
		if (xml.name().toString() == "SubTexture") {
			QXmlStreamAttributes attr = xml.attributes();
			QString name = attr.value("name").toString();
			int x = attr.value("x").toInt();
			int y = attr.value("y").toInt();
			int width = attr.value("width").toInt();
			int height = attr.value("height").toInt();
			rectMap[name] = QRect(x, y, width, height);
		}
	}
}

QImage TextureAtlas::getTexture(QString name) {
	if (name.isEmpty()) {
		return texture;
	}
	if (!rectMap.contains(name)) {
		Util::warn("No such texture: " + name);
		return texture;
	}
	QRect rect = rectMap[name];
	return texture.copy(rect);
}
