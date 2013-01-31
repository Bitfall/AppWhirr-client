#ifndef APPLICATIONDESCRIPTIONMODEL_H
#define APPLICATIONDESCRIPTIONMODEL_H

#include <QString>
#include <QSharedData>

#include "Application/ApplicationDescription/applicationstatesenum.h"

//
// consists from 2 parts: from a static, immutable part; and from a dynamic, mutable part (typically contains per-machine data and app-related settings)
//
// static means: it won't change unless the app is updated, or it won't change at all
//
// [!] This class uses internal-memory sharing for it's 2 parts. This allows to return ApplicationDescriptionModel in stack instead of in heap -> you can freely return it as a "non-pointer" object, and also this is the recommended way!
class ApplicationDescriptionModel
{
public:

    //
    // pathes (e.g. executablePath) are relative to the installedPath
    class StaticDescriptionModel : public QSharedData
    {
    public:
        StaticDescriptionModel();
        // copy constructor is also required by QSharedData (http://doc.qt.nokia.com/4.7/qshareddatapointer.html)
        StaticDescriptionModel(const StaticDescriptionModel &other);

        //
        // don't use this, unless you really know what you want to do
        //  use the DynamicDescriptionModel's method instead
        QString _getAppId() const;
        void _setAppId(QString appId);

        //
        // don't use this, unless you really know what you want to do
        //  use the DynamicDescriptionModel's method instead
        QString _getAppName() const;
        void _setAppName(QString appName);

        QString getExecutablePath() const;
        void setExecutablePath(QString appExecutablePath);

        QString getExecutableWorkingDirectory() const;
        void setExecutableWorkingDirectory(QString value);

        QString getSvgIconPath() const;
        void setSvgIconPath(QString path);

        QString getLargeIconPath() const;
        void setLargeIconPath(QString path);

        QString getMediumIconPath() const;
        void setMediumIconPath(QString path);

        QString getSmallIconPath() const;
        void setSmallIconPath(QString path);

        QString getLicenceFilePath() const;
        void setLicenceFilePath(QString value);


        //
        // checks:
        //  * title (only whether it's not empty)
        //  * executable path (only whether it's not empty)
        //  * and at least one of the icon-pathes have to be not empty
        //
        // If any of these checks fails it will return false. If all checks succeed it returns true.
//        bool getIsIconRelatedDataValid() const;


    private:
//        int __refCount;

        QString _appId;
        QString _appName;
        QString _executablePath;
        QString _executableWorkingPath;
        QString _licenceFilePath;

        QString _svgIconPath;
        QString _largeIconPath;
        QString _medimIconPath;
        QString _smallIconPath;
    };

    class DynamicDescriptionModel : public QSharedData
    {
    public:
        DynamicDescriptionModel();
        // copy constructor is also required by QSharedData (http://doc.qt.nokia.com/4.7/qshareddatapointer.html)
        DynamicDescriptionModel(const DynamicDescriptionModel &other);

        QString getAppId() const;
        void setAppId(QString appId);

        QString getAppName() const;
        void setAppName(QString appName);

        QString getInstalledRootPath() const;
        void setInstalledRootPath(QString installedRootPath);

        AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum getAppState() const;
        void setAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState);

        QString getInstalledVersion() const;
        void setInstalledVersion(QString _installedVersion);

        bool getIsStartWhenClientStarts() const;
        void setIsStartWhenClientStarts(bool value);

        bool getIsHidden() const;
        void setIsHidden(bool value);

        QString getSupportedPlatformFlags() const;
        void setSupportedPlatformFlags(QString value);

        int getRunCount() const;
        void setRunCount(int value);

        QString getGrabbedIconPath() const;
        void setGrabbedIconPath(QString value);

        QString getNewestAvailableVersionForThisPlatform() const;
        void setNewestAvailableVersionForThisPlatform(QString value);

        AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum getAppStatusInStore() const;
        void setAppStatusInStore(AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum value);

    private:
//        int __refCount;

        QString _appId;
        QString _appName;
        QString _installedRootPath;
        AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum _appState;
        QString _installedVersion;
        bool _isStartWhenClientStarts;
        QString _supportedPlatformFlags;
        bool _isHidden;
        int _runCount;
        QString _grabbedIconPath;
        QString _newestAvailableVersionForThisPlatform;
        AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum _appStatusInStore;
    };

public:
    explicit ApplicationDescriptionModel();
    // copy constructor is also required by QSharedData (http://doc.qt.nokia.com/4.7/qshareddatapointer.html)
    ApplicationDescriptionModel(const ApplicationDescriptionModel &other);

//    ApplicationDescriptionModel(const ApplicationDescriptionModel &appDescrModel);
//    ApplicationDescriptionModel &operator=(const ApplicationDescriptionModel &appDescrModel);
//    ~ApplicationDescriptionModel();

    // the ownership of the object will be managed by this object
    //  -> so the input HAVE TO BE an object in the heap (with new), and after giving it to this method you SHOULD NOT delete it. This object will, when it's time.
    void setStaticDescription(StaticDescriptionModel *staticDescriptionModel);
    void setDynamicDescription(DynamicDescriptionModel *dynamicDescriptionModel);

//    void setStaticDescription(QSharedDataPointer<StaticDescriptionModel> staticDescription);
//    void setDynamicDescription(QSharedDataPointer<DynamicDescriptionModel> dynamicDescription);

    //
    // these _ReadOnly() methods will detach the returned object before writing
    //  -> this means it won't modify this object's values!
    const QSharedDataPointer<StaticDescriptionModel> getStaticDescription_ReadOnly() const;
    const QSharedDataPointer<DynamicDescriptionModel> getDynamicDescription_ReadOnly() const;

    //
    // calling these _DataForModification() methods will detach the current data, then it will return it, so it can be safely modified
    StaticDescriptionModel* getStaticDescription_DataForModification();
    DynamicDescriptionModel* getDynamicDescription_DataForModification();

//    void _debugPrint();

    // -----------------------
    // --- utility methods ---
    bool isUpdateAvailable();
    bool isCompatibleWithTheCurrentPlatform();

private:
    QSharedDataPointer<StaticDescriptionModel> _staticDescription;
    QSharedDataPointer<DynamicDescriptionModel> _dynamicDescription;
};

#endif // APPLICATIONDESCRIPTIONMODEL_H
