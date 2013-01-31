#ifndef APPINSTALLSTATES_H
#define APPINSTALLSTATES_H

#include <QObject>

class AppInstallStates {
public:
    enum AppInstallStatesEnum {
        NoState = 0,

        PreparingBackup = 1,        // cleaning the backup directory before the actual backup
        PerformingBackup = 2,       // backup the app's files

        PreparingAppDirectory = 3,  // cleaning the app's (install) directory
        PerformingInstall = 4,      // the actual installation

        AfterInstallCleanup = 5,    // cleanup of temporary install files
        ValidatingAppInstall = 6    // validating the success of install
    };
};

#endif // APPINSTALLSTATES_H
