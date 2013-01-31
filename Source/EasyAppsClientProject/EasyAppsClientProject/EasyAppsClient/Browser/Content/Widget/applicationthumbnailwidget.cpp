#include "applicationthumbnailwidget.h"

#include <QPixmap>
#include <QImage>

#include "Common/Helpers/qimagehelper.h"

ApplicationThumbnailWidget::ApplicationThumbnailWidget(QString URL, bool isImage, QWidget *parent) :
    LayeredImagePresenterWidget(parent),
    URL(URL),
    isImage(isImage),
    selectWhenReady(false)
{
    this->setFixedSize(112, 69);

    this->connect(this, SIGNAL(clicked()), this, SLOT(clicked()));

    this->addImageLayer(ImageLayerModel());
}

void ApplicationThumbnailWidget::retrievedResourceForURL(QUrl URL, const QByteArray& bytes) {
    if (this->URL != URL.toString()) return;

    this->setImageLayer(0, LayeredImagePresenterWidget::ImageLayerModel(QImageHelper::createImageByByteArray(bytes, ImageTypes::PixelBased), LayeredImagePresenterWidget::ImageLayerAlignment::Center, LayeredImagePresenterWidget::ImageLayerResizeMode::NoResize));

    if (selectWhenReady)
        this->clicked();
}

void ApplicationThumbnailWidget::clicked() {
    Q_EMIT this->thumbnailSelected(URL, this->getPresentedImageAsPixmap(), isImage);
}

QSize ApplicationThumbnailWidget::sizeHint() const {
    return QSize(112, 69);
}

void ApplicationThumbnailWidget::setSelectedWhenResourceIsReady(bool value) {
    selectWhenReady = value;
}
