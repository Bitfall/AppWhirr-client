#ifndef INSTALLFALLBACKSTATESENUM_H
#define INSTALLFALLBACKSTATESENUM_H

class InstallFallbackStates {

public:

    enum InstallFallbackStatesEnum {
        NoAction = 0,
        SuccessfullyUncompressedZippedApplication,
        CopiedInstalledApplicationToTempDirectory,
        //        copiedDownloadedApplicationToApplicationDirectory,
        //        savedApplicationDatasToDatabase,
        RemovedTempFiles
    };

};

#endif // INSTALLFALLBACKSTATESENUM_H
