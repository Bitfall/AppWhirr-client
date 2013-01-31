#ifndef SVGTOPIXMAPRENDERER_H
#define SVGTOPIXMAPRENDERER_H

#include <QPixmap>
#include <QString>
#include <QSize>

class SvgToPixmapRenderer
{
private:
    SvgToPixmapRenderer();

public:
    static QPixmap renderSvgFromFileOrResourceToPixmap(QString fileNameOrResourceAlias, QSize pixmapTargetSize);
};

#endif // SVGTOPIXMAPRENDERER_H
