#include "qstringhelper.h"

#include <QStringList>

QStringHelper::QStringHelper()
{
}

QString QStringHelper::truncate(QString str, int length, QString tail, bool toChar, char c) {
    if (str.length() <= length) return str;

    QString truncated = str;
    truncated.truncate(length);
    if (toChar) {
        for (int i=length; i>0; --i) {
            if (truncated.at(i-1) == c) {
                truncated.truncate(i-1);
                break;
            }
        }
    }
    return truncated + tail;
}

bool QStringHelper::getFirstSectionByBeginEndIndicators(QString contentString, QString sectionBeginIndicator, QString sectionEndIndicator, QString &outSectionContent)
{
    int sectionBeginIndexInContent = contentString.indexOf(sectionBeginIndicator);
    int sectionEndIndexInContent = contentString.indexOf(sectionEndIndicator, sectionBeginIndexInContent + sectionBeginIndicator.length());


    if(sectionBeginIndexInContent < 0 || sectionEndIndexInContent < 0 || sectionEndIndexInContent < sectionBeginIndexInContent) {
        WLog("Section not found");
        return false;
    }

    outSectionContent = contentString.mid(sectionBeginIndexInContent + sectionBeginIndicator.length(), sectionEndIndexInContent - sectionBeginIndexInContent - sectionBeginIndicator.length());

    return true;
}

QString QStringHelper::convertQStringListToQuotedSpaceSeparatedQString(const QStringList &stringList)
{
    QString _ret;
    for(int i = 0; i < stringList.size(); ++i) {
        if(i == 0) {
            _ret += QString("\"%1\"").arg(stringList[i]);
        }
        else {
            // with additional leading space
            _ret += QString(" \"%1\"").arg(stringList[i]);
        }
    }
    return _ret;
}

#ifdef Q_OS_MAC
QString QStringHelper::CFStringToQString(CFStringRef s) {
    QString result;

    if (s != NULL) {
        CFIndex length = 2*(CFStringGetLength(s) + 1); // Worst case for UTF8
        char* buffer = new char[length];
        if (CFStringGetCString(s, buffer, length, kCFStringEncodingUTF8)) {
            result = QString::fromUtf8(buffer);
        }
        delete buffer;
    }

    return result;
}
#endif
