#include "qsizehelper.h"

#include <math.h>

QSizeHelper::QSizeHelper()
{
}

QSize QSizeHelper::multiplyQSizeWithNumber(QSize s, double n)
{
    return QSize((int)((double)s.width()) * n, (int)((double)s.height()) * n);
}

//QSize QSizeHelper::multiplyQSizeWithNumber(QSize s, float n)
//{
//    return QSize(s.width() * n, s.height() * n);
//}

QSize QSizeHelper::normalizedIntBasedSizeFromFloatBasedSize(QSizeF floatBasedSize, bool isRoundIt)
{
    int w = (isRoundIt ? awRoundd(floatBasedSize.width()) : floatBasedSize.width());
    int h = (isRoundIt ? awRoundd(floatBasedSize.height()) : floatBasedSize.height());
    return QSize((w < 0 ? 0 : w), (h < 0 ? 0 : h));
}
