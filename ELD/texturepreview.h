#ifndef TEXTUREPREVIEW_H
#define TEXTUREPREVIEW_H

#include "textureatlas.h"

#include <QDialog>

namespace Ui {
class TexturePreview;
}

class TexturePreview : public QDialog
{
    Q_OBJECT

public:
    explicit TexturePreview(QString atlasUrl, TextureAtlas *textureAtlas, QWidget *parent = 0);
    ~TexturePreview();

    static TexturePreview *dialog;
    static QString selectTexture(QString atlasUrl, TextureAtlas *textureAtlas, QWidget *parent = 0);

    void readTextureAtlas();
    QString selectedTexture();

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);

private:
        Ui::TexturePreview *ui;
        QString currentAtlasUrl;
        TextureAtlas *currentTextureAtlas;
};

#endif // TEXTUREPREVIEW_H
