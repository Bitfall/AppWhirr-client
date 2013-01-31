#ifndef QCOLORHELPER_H
#define QCOLORHELPER_H

#include <QColor>

class QColorHelper
{
private:
    QColorHelper();

public:
    static QColor blendColors(QColor color1, QColor color2, float blendFactor);
};

#endif // QCOLORHELPER_H
