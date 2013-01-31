#include "qstringhelper.h"

QStringHelper::QStringHelper()
{
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

//bool QStringHelper::isEqualFromCharacterToCharacter(QString stringOne, QString stringTwo)
//{
//    bool isSectionContentMatch = true;
//    if(stringOne.size() == stringTwo.size()) {
//        DLog("Size match.");
//        for(int ci = 0; ci < stringOne.size() && isSectionContentMatch; ci++) {
//            if(stringOne[ci] != stringTwo[ci]) {
//                DLog("Mismatch at index, character: ") << ci << stringOne[ci] << stringTwo[ci];
//                isSectionContentMatch = false;
//            }
//        }
//    }
//    else {
//        DLog("String sizes does NOT match: ") << stringOne.size() << stringTwo.size();
//        return false;
//    }

//    return isSectionContentMatch;
//}
