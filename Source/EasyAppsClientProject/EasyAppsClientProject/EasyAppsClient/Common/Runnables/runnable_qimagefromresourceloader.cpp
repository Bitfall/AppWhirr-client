#include "runnable_qimagefromresourceloader.h"

#include <QImage>

Runnable_QImageFromResourceLoader::Runnable_QImageFromResourceLoader(QByteArray imageResource, QObject *parent) :
    QObject(parent),
  _imgResource(imageResource)
{
}

void Runnable_QImageFromResourceLoader::run()
{
    QImage img = QImage::fromData(_imgResource);
    Q_EMIT imageLoaded(img);
}
