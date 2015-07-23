#include "config.h"
#include "texturepreview.h"
#include "ui_texturepreview.h"

TexturePreview *TexturePreview::dialog;

QString TexturePreview::selectTexture(QString atlasUrl, TextureAtlas *textureAtlas, QWidget *parent) {
    TexturePreview::dialog = new TexturePreview(atlasUrl, textureAtlas, parent);
    int code = TexturePreview::dialog->exec();

    QString data;
    if (code == Rejected)
        data = atlasUrl;
    else
        data = TexturePreview::dialog->selectedTexture();

    delete TexturePreview::dialog;
    return data;
}

TexturePreview::TexturePreview(QString atlasUrl, TextureAtlas *textureAtlas, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TexturePreview)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(1, 1);
    ui->splitter->setStretchFactor(1, 2);

    currentAtlasUrl = atlasUrl;
    currentTextureAtlas = textureAtlas;
    readTextureAtlas();
}

TexturePreview::~TexturePreview()
{
    delete ui;
}

void TexturePreview::readTextureAtlas() {
    ui->comboBox->addItems(currentTextureAtlas->getTextureNames());
}

QString TexturePreview::selectedTexture() {
    return currentAtlasUrl + ";" + ui->comboBox->currentText();
}

void TexturePreview::on_comboBox_currentTextChanged(const QString &arg1)
{
    QImage texture = currentTextureAtlas->getTexture(arg1);
    ui->label->setPixmap(QPixmap::fromImage(texture));
    ui->label->update();
}
