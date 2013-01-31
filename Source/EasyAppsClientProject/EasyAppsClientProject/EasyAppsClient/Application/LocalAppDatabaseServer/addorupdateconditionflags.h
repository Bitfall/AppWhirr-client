#ifndef ADDORUPDATECONDITIONFLAGS_H
#define ADDORUPDATECONDITIONFLAGS_H

namespace AppWhirr
{


class UpdateStateCondition {

public:
    //
    //  * UpdateIfNewState... -> don't update if it's not true
    enum UpdateStateConditionFlags {
        USC_NoConditions = 0,
        USC_UpdateIfNewStateLower = 1,
        USC_UpdateIfNewStateEqual = 2,
        USC_UpdateIfNewStateHigher = 4,

        USC_UpdateAnyway = 7 // contains every flag
    };

};


class UpdateAppDetailPartsCondition {

public:
    //
    // Notes about names:
    //  * Skip...IfUpdate -> skip this parameter, only if it's an update, apply it if it's an add/insert
    //
    enum UpdateAppDetailPartsConditionFlags {
        NoFlags = 0,
        SkipSupportedPlatformFlagsIfUpdate = 1,
        SkipIsHiddenIfUpdate = 2,
        SkipIsStartWhenClientStartsIfUpdate = 4,
        SkipRunCountIfUpdate = 8,
        SkipGrabbedIconPathIfUpdate = 16,
        SkipInstalledVersionIfUpdate = 32,
        SkipNewestVersionForThisPlatformIfUpdate = 64,
        SkipInstalledRootPathIfUpdate = 128,
        SkipAppStateCodeIfUpdate = 256,
        SkipStaticAppInfosIfUpdate = 512, // will skip the whole static app infos part if it's just an update
        SkipAppStatusInStoreIfUpdate = 1024
    };

};

}

#endif // ADDORUPDATECONDITIONFLAGS_H
