#ifndef QSTRINGHELPER_H
#define QSTRINGHELPER_H

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

#include <QString>


class QStringHelper
{
private:
    explicit QStringHelper();

public:

    static QString truncate(QString str, int length, QString tail = "...", bool toChar = true, char c = ' ');

    //
    // searches in "contentString" for a section starts with "sectionBeginIndicator" (string) and end with "sectionEndIndicator" (string)
    //  it will stop for the first valid section
    // will return whether it found the section
    //  if founds returns true and copies the section-content to outSectionContent
    static bool getFirstSectionByBeginEndIndicators(QString contentString, QString sectionBeginIndicator, QString sectionEndIndicator, QString &outSectionContent);

    /*!
        Example:
        * stringList = {"hello", "world"}
        ** returned string: QString("\"hello\" \"world\"")
    */
    static QString convertQStringListToQuotedSpaceSeparatedQString(const QStringList &stringList);

#ifdef Q_OS_MAC
    static QString CFStringToQString(CFStringRef s);
#endif
};

#endif // QSTRINGHELPER_H
