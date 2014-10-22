#include <QImage>
#include <QString>
#include <QHash>

#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

class TextureAtlas {
  public:
	TextureAtlas(QString path);

	void parseAtlas(QString path);
	QImage getTexture(QString name);

  protected:
	QImage texture;
	QHash<QString, QRect> rectMap;

};

#endif // TEXTUREATLAS_H
