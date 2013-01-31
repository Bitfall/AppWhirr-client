#ifndef QPIXMAPHELPER_H
#define QPIXMAPHELPER_H

#include <QPixmap>

class QPixmapHelper
{
private:
    QPixmapHelper();

public:
    static QPixmap createPixmapFromByteArray(QByteArray pixmapByteArray);
};

#endif // QPIXMAPHELPER_H
