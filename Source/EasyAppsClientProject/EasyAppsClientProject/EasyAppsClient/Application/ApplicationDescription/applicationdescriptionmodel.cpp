#include "applicationdescriptionmodel.h"

#include "Common/System/systemprofiler.h"


// ===================================
// === ApplicationDescriptionModel ===

ApplicationDescriptionModel::ApplicationDescriptionModel()
{
    this->_staticDescription = new StaticDescriptionModel;
    this->_dynamicDescription = new DynamicDescriptionModel;
}

ApplicationDescriptionModel::ApplicationDescriptionModel(const ApplicationDescriptionModel &other) :
    _staticDescription(other._staticDescription),
    _dynamicDescription(other._dynamicDescription)
{
}

void ApplicationDescriptionModel::setStaticDescription(ApplicationDescriptionModel::StaticDescriptionModel *staticDescriptionModel)
{
    this->_staticDescription = staticDescriptionModel;
}

void ApplicationDescriptionModel::setDynamicDescription(ApplicationDescriptionModel::DynamicDescriptionModel *dynamicDescriptionModel)
{
    this->_dynamicDescription = dynamicDescriptionModel;
}

//void ApplicationDescriptionModel::setStaticDescription(QSharedDataPointer<ApplicationDescriptionModel::StaticDescriptionModel> staticDescription)
//{
//    this->_staticDescription = staticDescription;
//}

//void ApplicationDescriptionModel::setDynamicDescription(QSharedDataPointer<ApplicationDescriptionModel::DynamicDescriptionModel> dynamicDescription)
//{
//    this->_dynamicDescription = dynamicDescription;
//}

const QSharedDataPointer<ApplicationDescriptionModel::StaticDescriptionModel> ApplicationDescriptionModel::getStaticDescription_ReadOnly() const
{
    return this->_staticDescription;
//    return this->_staticDescription.data();
}

const QSharedDataPointer<ApplicationDescriptionModel::DynamicDescriptionModel> ApplicationDescriptionModel::getDynamicDescription_ReadOnly() const
{
    return this->_dynamicDescription;
//    return this->_dynamicDescription.data();
}

ApplicationDescriptionModel::StaticDescriptionModel* ApplicationDescriptionModel::getStaticDescription_DataForModification() {
    return this->_staticDescription.data();
}

ApplicationDescriptionModel::DynamicDescriptionModel* ApplicationDescriptionModel::getDynamicDescription_DataForModification() {
    return this->_dynamicDescription.data();
}

// ==========================================
// === ApplicationDynamicDescriptionModel ===

ApplicationDescriptionModel::DynamicDescriptionModel::DynamicDescriptionModel() :
    _appState(AppWhirr::ApplicationGrabStates::NoState),
    _isStartWhenClientStarts(false),
    _isHidden(false),
    _runCount(0),
    _appStatusInStore(AppWhirr::ApplicationStatusInStore::NoAppStatusInStore)
{
}

ApplicationDescriptionModel::DynamicDescriptionModel::DynamicDescriptionModel(const DynamicDescriptionModel &other) :
    QSharedData(other),
    _appId(other._appId),
    _appName(other._appName),
    _installedRootPath(other._installedRootPath),
    _appState(other._appState),
    _installedVersion(other._installedVersion),
    _isStartWhenClientStarts(other._isStartWhenClientStarts),
    _isHidden(other._isHidden),
    _supportedPlatformFlags(other._supportedPlatformFlags),
    _runCount(other._runCount),
    _grabbedIconPath(other._grabbedIconPath),
    _newestAvailableVersionForThisPlatform(other._newestAvailableVersionForThisPlatform),
    _appStatusInStore(other._appStatusInStore)
{

}

QString ApplicationDescriptionModel::DynamicDescriptionModel::getAppId() const {
    return this->_appId;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setAppId(QString appId) {
    this->_appId = appId;
}

QString ApplicationDescriptionModel::DynamicDescriptionModel::getAppName() const {
    return this->_appName;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setAppName(QString appName) {
    this->_appName = appName;
}

QString ApplicationDescriptionModel::DynamicDescriptionModel::getInstalledRootPath() const {
    return this->_installedRootPath;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setInstalledRootPath(QString installedPath) {
    this->_installedRootPath = installedPath;
}

AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum ApplicationDescriptionModel::DynamicDescriptionModel::getAppState() const {
    return this->_appState;
}
void ApplicationDescriptionModel::DynamicDescriptionModel::setAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState) {
    this->_appState = appState;
}

QString ApplicationDescriptionModel::DynamicDescriptionModel::getInstalledVersion() const {
    return this->_installedVersion;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setInstalledVersion(QString installedVersion) {
    this->_installedVersion = installedVersion;
}

bool ApplicationDescriptionModel::DynamicDescriptionModel::getIsStartWhenClientStarts() const {
    return this->_isStartWhenClientStarts;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setIsStartWhenClientStarts(bool value) {
    this->_isStartWhenClientStarts = value;
}

bool ApplicationDescriptionModel::DynamicDescriptionModel::getIsHidden() const {
    return this->_isHidden;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setIsHidden(bool value) {
    this->_isHidden = value;
}

QString ApplicationDescriptionModel::DynamicDescriptionModel::getSupportedPlatformFlags() const {
    return this->_supportedPlatformFlags;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setSupportedPlatformFlags(QString value) {
    this->_supportedPlatformFlags = value;
}

int ApplicationDescriptionModel::DynamicDescriptionModel::getRunCount() const {
    return this->_runCount;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setRunCount(int value) {
    this->_runCount = value;
}

QString ApplicationDescriptionModel::DynamicDescriptionModel::getGrabbedIconPath() const {
    return this->_grabbedIconPath;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setGrabbedIconPath(QString value) {
    this->_grabbedIconPath = value;
}

QString ApplicationDescriptionModel::DynamicDescriptionModel::getNewestAvailableVersionForThisPlatform() const {
    return this->_newestAvailableVersionForThisPlatform;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setNewestAvailableVersionForThisPlatform(QString value) {
    this->_newestAvailableVersionForThisPlatform = value;
}

AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum ApplicationDescriptionModel::DynamicDescriptionModel::getAppStatusInStore() const {
    return this->_appStatusInStore;
}

void ApplicationDescriptionModel::DynamicDescriptionModel::setAppStatusInStore(AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum value) {
    this->_appStatusInStore = value;
}

// =========================================
// === ApplicationStaticDescriptionModel ===

ApplicationDescriptionModel::StaticDescriptionModel::StaticDescriptionModel()
{
}

ApplicationDescriptionModel::StaticDescriptionModel::StaticDescriptionModel(const StaticDescriptionModel &other) :
    QSharedData(other),

    _appId(other._appId),
    _appName(other._appName),
    _executablePath(other._executablePath),
    _executableWorkingPath(other._executableWorkingPath),
    _svgIconPath(other._svgIconPath),
    _largeIconPath(other._largeIconPath),
    _medimIconPath(other._medimIconPath),
    _smallIconPath(other._smallIconPath),
    _licenceFilePath(other._licenceFilePath)
{

}

QString ApplicationDescriptionModel::StaticDescriptionModel::_getAppId() const {
    return this->_appId;
}

void ApplicationDescriptionModel::StaticDescriptionModel::_setAppId(QString appId) {
    this->_appId = appId;
}

//bool ApplicationDescriptionModel::ApplicationStaticDescriptionModel::getIsIconRelatedDataValid() const
//{
//    return (!_appName.isEmpty() && !_executablePath.isEmpty() &&
//            (!svgIconPath.isEmpty() || !largeIconPath.isEmpty() || !medimIconPath.isEmpty() || !smallIconPath.isEmpty()));
//}

QString ApplicationDescriptionModel::StaticDescriptionModel::getSvgIconPath() const
{
    return _svgIconPath;
}

QString ApplicationDescriptionModel::StaticDescriptionModel::getLargeIconPath() const
{
    return _largeIconPath;
}

QString ApplicationDescriptionModel::StaticDescriptionModel::getMediumIconPath() const
{
    return _medimIconPath;
}

QString ApplicationDescriptionModel::StaticDescriptionModel::getSmallIconPath() const
{
    return _smallIconPath;
}

QString ApplicationDescriptionModel::StaticDescriptionModel::getLicenceFilePath() const {
    return this->_licenceFilePath;
}
void ApplicationDescriptionModel::StaticDescriptionModel::setLicenceFilePath(QString value) {
    this->_licenceFilePath = value;
}

void ApplicationDescriptionModel::StaticDescriptionModel::setSvgIconPath(QString path)
{
    this->_svgIconPath = path;
}

void ApplicationDescriptionModel::StaticDescriptionModel::setLargeIconPath(QString path)
{
    this->_largeIconPath = path;
}

void ApplicationDescriptionModel::StaticDescriptionModel::setMediumIconPath(QString path)
{
    this->_medimIconPath = path;
}

void ApplicationDescriptionModel::StaticDescriptionModel::setSmallIconPath(QString path)
{
    this->_smallIconPath = path;
}

QString ApplicationDescriptionModel::StaticDescriptionModel::_getAppName() const
{
    return _appName;
}

QString ApplicationDescriptionModel::StaticDescriptionModel::getExecutablePath() const {
    return _executablePath;
}

QString ApplicationDescriptionModel::StaticDescriptionModel::getExecutableWorkingDirectory() const {
    return this->_executableWorkingPath;
}

void ApplicationDescriptionModel::StaticDescriptionModel::setExecutableWorkingDirectory(QString value) {
    this->_executableWorkingPath = value;
}

void ApplicationDescriptionModel::StaticDescriptionModel::_setAppName(QString appName) {
    this->_appName = appName;
}

void ApplicationDescriptionModel::StaticDescriptionModel::setExecutablePath(QString appExecutablePath) {
    this->_executablePath = appExecutablePath;
}


// -----------------------
// --- utility methods ---

bool ApplicationDescriptionModel::isUpdateAvailable()
{
    if(this->getDynamicDescription_ReadOnly()->getAppState() == AppWhirr::ApplicationGrabStates::Installed)
    {
        if(this->getDynamicDescription_ReadOnly()->getInstalledVersion() != this->getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform())
        {
            if(this->getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform().isEmpty()) {
                WLog("Newest available version - is empty!");
                return false;
            }
            if(this->getDynamicDescription_ReadOnly()->getInstalledVersion().isEmpty()) {
                WLog("Installed version - is empty!");
            }
            return true;
        }
    }

    return false;
}

bool ApplicationDescriptionModel::isCompatibleWithTheCurrentPlatform()
{
    // currently this platform-flags only indicates whether the current platform is supported
    return (this->getDynamicDescription_ReadOnly()->getSupportedPlatformFlags() != "0");

//    bool isCompatible = false;
//    PlatformFlagsHelper::SummedSinglePlatformInformations *appCurrentPlatformInfo = NULL;
//    quint64 appCurrPlatformSummedSubFlags = this->getDynamicDescription_ReadOnly()->getSupportedPlatformFlags().toULongLong();
////    DLog("App summed sub flags: ") << appCurrPlatformSummedSubFlags;

//#ifdef Q_WS_WIN
//    appCurrentPlatformInfo = PlatformFlagsHelper::SummedWinPlatformInformations::_createBySummedSubFlags(appCurrPlatformSummedSubFlags);
//#endif

//#ifdef Q_WS_MAC
//    appCurrentPlatformInfo = PlatformFlagsHelper::SummedMacPlatformInformations::_createBySummedSubFlags(appCurrPlatformSummedSubFlags);
//#endif

//    if(appCurrentPlatformInfo != NULL) {
//        isCompatible = SystemProfiler::sharedSystemProfiler()->isThisInfoIsCompatibleWithThisPlatform(appCurrentPlatformInfo);
//        delete appCurrentPlatformInfo;
//    }

//    return isCompatible;
}
