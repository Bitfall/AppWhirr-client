#ifndef QSIZEHELPER_H
#define QSIZEHELPER_H

#include <QSize>

class QSizeHelper
{
private:
    QSizeHelper();

public:
    static QSize multiplyQSizeWithNumber(QSize s, double n);
//    static QSize multiplyQSizeWithNumber(QSize s, float n);
    static QSize normalizedIntBasedSizeFromFloatBasedSize(QSizeF floatBasedSize, bool isRoundIt = true);
};

#endif // QSIZEHELPER_H
