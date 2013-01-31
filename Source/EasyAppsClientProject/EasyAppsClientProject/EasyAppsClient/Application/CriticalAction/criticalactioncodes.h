#ifndef CRITICALACTIONCODES_H
#define CRITICALACTIONCODES_H

namespace AppWhirr {

class CriticalActionCodes {
public:
    enum CriticalActionCodesEnum {
        Invalid = 0,

        AppInstallOrUpdateActive = 1,
        AppUninstallActive = 2,
        UserGrabSyncActive = 3,
        UserOfflineAppListSyncActive = 4
    };
};

}

#endif // CRITICALACTIONCODES_H
