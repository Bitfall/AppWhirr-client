#ifndef TEXTSIZEHELPER_H
#define TEXTSIZEHELPER_H

#include <QString>
#include <QFont>
#include <QSize>

class TextSizeHelper
{
private:
    TextSizeHelper();

public:
    static QSize getTextSizeWithFont(QString text, QFont font);
};

#endif // TEXTSIZEHELPER_H
