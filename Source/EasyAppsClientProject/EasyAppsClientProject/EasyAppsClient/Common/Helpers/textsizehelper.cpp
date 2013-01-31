#include "textsizehelper.h"

#include <QFontMetrics>

TextSizeHelper::TextSizeHelper()
{
}

QSize TextSizeHelper::getTextSizeWithFont(QString text, QFont font)
{
    QFontMetrics fm(font);
    int pixelsWide = fm.width(text);
    int pixelsHigh = fm.height();

    return QSize(pixelsWide, pixelsHigh);
}
