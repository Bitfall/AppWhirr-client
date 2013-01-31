#include "qcolorhelper.h"

QColorHelper::QColorHelper()
{
}

QColor QColorHelper::blendColors(QColor color1, QColor color2, float blendFactor) {
    QColor _ret(255.0f * (color1.redF() + (color2.redF() - color1.redF()) * blendFactor),
                255.0f * (color1.greenF() + (color2.greenF() - color1.greenF()) * blendFactor),
                255.0f * (color1.blueF() + (color2.blueF() - color1.blueF()) * blendFactor));

    return _ret;
}
