#ifndef APPLICATIONSTATESENUM_H
#define APPLICATIONSTATESENUM_H

namespace AppWhirr {

class ApplicationGrabStates {
public:
    enum ApplicationGrabStatesEnum {
        // basic, not grabbed states
        NoState = 0,                    // There is no info in the database
        StateRequestError = 1,                      // error during the state-request

        // grabbed states
        GrabbedButNotInstalled = 10,     // The app is not installed in this machine, but scanned and ready for presenting in MyApps page
        GrabbedAndInstalling = 11,        // It's currently installing
        GrabbedAndUninstalling = 12,

        // installed states
        // !!! Don't change the order of these 2 - it can affect a lot of parts !!!
        Installed = 20,     // Application is installed and it's the newest version
        //    InstalledButUpdateAvailable = 21            // Application is installed, but there is a newer version
        InstalledAndUpdating = 21
    };

};

class ApplicationSubStateFlags {
public:
    enum ApplicationSubStateFlagsEnum {
        NoState = 0, // There is no info in the database
        UpdateAvailable = 1,
        NotAvailableOnThisPlatform = 2,
        AppRemovedFromTheSystem = 4 // if the developer removed / deleted it
    };

    typedef int SummedApplicationSubStateFlags; // sum of ApplicationSubStatesEnum s
};

class ApplicationStatusInStore {
public:
    enum ApplicationStatusInStoreEnum {
        NoAppStatusInStore = 0,
        CurrentInStore = 100,
        RemovedFromStore = 300,
        TheClientApp = 500
    };
};

}

#endif // APPLICATIONSTATESENUM_H
