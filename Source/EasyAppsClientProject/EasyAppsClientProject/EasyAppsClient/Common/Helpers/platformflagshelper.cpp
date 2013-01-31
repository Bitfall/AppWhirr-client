#include "platformflagshelper.h"

PlatformFlagsHelper::PlatformFlagsHelper()
{
}

bool PlatformFlagsHelper::isTheseInfosAreCompatible(SummedSinglePlatformInformations *info1, SummedSinglePlatformInformations *info2)
{
//    DLog("Info 1 summed subflags: ") << info1->_getSummedSubFlags();
//    DLog("Info 2 summed subflags: ") << info2->_getSummedSubFlags();

    // compare the 'MainOS' part
    //  Mac or Windows or both
    if( (info1->getMainOsFlag() & info2->getMainOsFlag()) == 0 ) {
//        DLog("Main OS flag is not compatible.");
        return false;
    }

    // compare the 'SummedBitness' part
    //  32bit or 64bit or both
    if( (info1->getSummedBitnessFlags() & info2->getSummedBitnessFlags()) == 0 ) {//info1->getSummedBitnessFlags() & info2->getSummedBitnessFlags()) <= 0 ) {
//        DLog("Bitness flag is not compatible.") << info1->getSummedBitnessFlags() << info2->getSummedBitnessFlags();
        return false;
    }

    // compare the 'SummedOsSubFlags' part
    //  can be (Windows) XP SP3, Vista, Vista SP1, ... (Mac) 10.6.0, 10.6.1, ...
    //  the os-sub-flags depends on the main-os-flag
    if( (info1->getSummedOsSubFlags() & info2->getSummedOsSubFlags()) == 0) {//) <= 0 ) {
//        DLog("OS sub flag is not compatible.");
        return false;
    }

    return true;
}

QString PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(quint64 intBasedFlags)
{
    return QString::number(intBasedFlags);
}
