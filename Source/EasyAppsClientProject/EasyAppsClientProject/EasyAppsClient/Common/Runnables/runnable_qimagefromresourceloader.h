#ifndef RUNNABLE_QIMAGEFROMRESOURCELOADER_H
#define RUNNABLE_QIMAGEFROMRESOURCELOADER_H

#include <QObject>
#include <QRunnable>
#include <QImage>

/*! Can load+create a QImage from a byte-array resource-data in background (async)

    @note Don't create QPixmap in background thread! It requires main-thread dependent operations, and it's even forbidden in Mac OS X (it won't create a valid QPixmap in background thread). Instead you should create a QImage with this background/runnable class and create the QPixmap from QImage in the main-thread ( with QPixmap::fromImage(qimageobj) )
*/
class Runnable_QImageFromResourceLoader : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Runnable_QImageFromResourceLoader(QByteArray imageResource, QObject *parent = 0);
    
    void run();

Q_SIGNALS:
    void imageLoaded(QImage image);

private:
    QByteArray _imgResource;
};

#endif // RUNNABLE_QIMAGEFROMRESOURCELOADER_H
