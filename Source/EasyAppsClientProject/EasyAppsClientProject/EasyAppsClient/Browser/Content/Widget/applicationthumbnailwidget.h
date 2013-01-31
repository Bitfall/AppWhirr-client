#ifndef APPLICATIONTHUMBNAILWIDGET_H
#define APPLICATIONTHUMBNAILWIDGET_H

#include <QWidget>
#include <QUrl>

#include "Common/Widgets/layeredimagepresenterwidget.h"

QT_BEGIN_NAMESPACE
class QSize;
QT_END_NAMESPACE

class ApplicationThumbnailWidget : public LayeredImagePresenterWidget
{
    Q_OBJECT
public:
    explicit ApplicationThumbnailWidget(QString URL, bool isImage = true, QWidget *parent = 0);

    QSize sizeHint() const;
    void setSelectedWhenResourceIsReady(bool value);

Q_SIGNALS:
    void thumbnailSelected(QString url, QPixmap thumbnailPixmap, bool image);

public Q_SLOTS:
    void retrievedResourceForURL(QUrl URL, const QByteArray& bytes);
    void clicked();

private:
    QString                 URL;
    bool                    isImage;
    bool                    selectWhenReady;
};

#endif // APPLICATIONTHUMBNAILWIDGET_H
