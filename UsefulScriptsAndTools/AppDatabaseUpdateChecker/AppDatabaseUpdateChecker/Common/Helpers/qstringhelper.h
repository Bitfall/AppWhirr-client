#ifndef QSTRINGHELPER_H
#define QSTRINGHELPER_H

#include <QString>

class QStringHelper
{
private:
    QStringHelper();

public:

    //
    // will return whether it found the section
    //  if founds returns true and copies the section-content to outSectionContent
    static bool getFirstSectionByBeginEndIndicators(QString contentString, QString sectionBeginIndicator, QString sectionEndIndicator, QString &outSectionContent);

    //
    // Qt's compare cannot compare some complicated strings (like HTML code sections). It does some fast-compare and sometimes fails for complicated (e.g. multiline) strings.
    //  So use this compare for a character-by-character compare of 2 strings. Slower but the returned result is correct for complicated strings as well.
//    static bool isEqualFromCharacterToCharacter(QString stringOne, QString stringTwo);
};

#endif // QSTRINGHELPER_H
