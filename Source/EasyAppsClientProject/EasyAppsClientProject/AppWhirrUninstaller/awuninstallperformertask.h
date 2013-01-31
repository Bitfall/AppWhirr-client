#ifndef AWUNINSTALLPERFORMERTASK_H
#define AWUNINSTALLPERFORMERTASK_H

#include "../EasyAppsClient/Common/awtask.h"

#include <QFileInfo>

class UninstallConfigs;
class AWUninstallPerformerTask : public AWTask
{
    Q_OBJECT
public:
    explicit AWUninstallPerformerTask(UninstallConfigs *uninstallConfigs, QObject *parent = 0);

    void run();
    
Q_SIGNALS:
    void uninstallStateMessage(QString stateMessage);
    void finished();

private:
    void startTheUninstall();
    //
    void removeInstalledApps();
    void finishedWithRemoveInstalledApps();
    //
    void removeAppWhirrClient();
    void finishedWithRemoveAppWhirrClient();
    //
    void removeAppWhirrLocalFiles();
    void finishedWithRemoveAppWhirrLocalFiles();
    //
    void startOSSpecificCleanup_PreClientRemove();
    void startOSSpecificCleanup_PostClientRemove();
#ifdef Q_OS_WIN
    bool win_OSSpecificCleanup_PreClientRemove();
    bool win_OSSpecificCleanup_PostClientRemove();
#endif
#ifdef Q_OS_MAC
    bool mac_OSSpecificCleanup_PreClientRemove();
    bool mac_OSSpecificCleanup_PostClientRemove();
#endif
    void finishedWithOSSpecificCleanup_PreClientRemove();
    void finishedWithOSSpecificCleanup_PostClientRemove();

    void finishedWithUninstall();

private:
    bool deleteFileOrFolderWithBackup(QFileInfo fileOrFolderFileInfoToBeDeleted);
    bool removeFileByPath(QString filePathString);

private:
    UninstallConfigs *_uninstallConfigs;
};

#endif // AWUNINSTALLPERFORMERTASK_H
