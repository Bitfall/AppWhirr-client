#include "platformflagshelper.h"

#define kFlag32BitSupport       1;
#define kFlag64BitSupport       2;
//#define kFlagWindows            4;
//#define kFlagMacOS              8;

PlatformFlagsHelper::PlatformFlagsHelper()
{
}

//quint64 PlatformFlagsHelper::_buildSummedPlatformFlagsByComponentsAsInt(PlatformFlagsOS os, PlatformFlagsBitness bitness)
//{
//    quint64 flags = 0;

//    if (bitness == Bit32) {
//        flags |= kFlag32BitSupport;
//    } else if(bitness == Bit64) {
//        flags |= kFlag64BitSupport;
//    }

////    if(os == Mac) {
////        flags |= kFlagMacOS;
////    }
////    else if(os == Windows) {
////        flags |= kFlagWindows;
////    }

//    return flags;
//}

//QString PlatformFlagsHelper::buildSummedPlatformFlagsByComponents(PlatformFlagsOS os, PlatformFlagsBitness bitness)
//{
//    // temporarily this is simply a conversion of the int version, but the string version is more flexible and can handle much more

//    return _convertIntBasedFlagsToStringBased(_buildSummedPlatformFlagsByComponentsAsInt(os, bitness));
//}

//bool PlatformFlagsHelper::isTheseFlagsAreCompatible(QString firstFlag, QString secondFlag)
//{
//    // temporarily this is based on the int version

//    quint64 firstFlagInt = firstFlag.toULongLong();
//    quint64 secondFlagInt = secondFlag.toULongLong();

//    return _isTheseFlagsAreCompatibleIntVersion(firstFlagInt, secondFlagInt);
//}

//bool PlatformFlagsHelper::_isTheseFlagsAreCompatibleIntVersion(quint64 firstFlagInt, quint64 secondFlagInt)
//{
//    quint64 comparedFlags = firstFlagInt & secondFlagInt;

//    quint64 summedWinFlags = __getAllWinFlagsSummed();
//    quint64 summedOsFlags = kFlagWindows; summedOsFlags |= kFlagMacOS;
//    quint64 summedBitnessFlags = kFlag32BitSupport; summedBitnessFlags |= kFlag64BitSupport;

//    return (((comparedFlags & summedOsFlags) > 0) && ((comparedFlags & summedBitnessFlags) > 0));
//}

bool PlatformFlagsHelper::isTheseInfosAreCompatible(SummedSinglePlatformInformations *info1, SummedSinglePlatformInformations *info2)
{
//    DLog("Info 1 summed subflags: ") << info1->_getSummedSubFlags();
//    DLog("Info 2 summed subflags: ") << info2->_getSummedSubFlags();

    if(info1->getMainOsFlag() != info2->getMainOsFlag()) {
        DLog("Main OS flag is not compatible.");
        return false;
    }

    if( (info1->getSummedBitnessFlags() & info2->getSummedBitnessFlags()) <= 0 ) {
        DLog("Bitness flag is not compatible.") << info1->getSummedBitnessFlags() << info2->getSummedBitnessFlags();
        return false;
    }

    if( (info1->getSummedOsSubFlags() & info2->getSummedOsSubFlags()) <= 0 ) {
        DLog("OS sub flag is not compatible.");
        return false;
    }

    return true;
}

QString PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(quint64 intBasedFlags)
{
    return QString::number(intBasedFlags);
}


quint64 PlatformFlagsHelper::__getAllMacFlagsSummed() {
    quint64 summed = 0;
    summed |= MacOS_10_6_0;
    summed |= MacOS_10_6_1;
    summed |= MacOS_10_6_2;
    summed |= MacOS_10_6_3;
    summed |= MacOS_10_6_4;
    summed |= MacOS_10_6_5;
    summed |= MacOS_10_6_6;
    summed |= MacOS_10_6_7;
    summed |= MacOS_10_6_8;

    summed |= MacOS_10_7_0;
    summed |= MacOS_10_7_1;
    summed |= MacOS_10_7_2;

    return summed;
}

quint64 PlatformFlagsHelper::__getAllWinFlagsSummed() {
    int summed = 0;
    summed |= Win_XP_NoSP;
    summed |= Win_XP_SP1;
    summed |= Win_XP_SP2;
    summed |= Win_XP_SP3;

    summed |= Win_Vista_NoSP;
    summed |= Win_Vista_SP1;
    summed |= Win_Vista_SP2;

    summed |= Win_7_NoSP;
    summed |= Win_7_SP1;

    return summed;
}
