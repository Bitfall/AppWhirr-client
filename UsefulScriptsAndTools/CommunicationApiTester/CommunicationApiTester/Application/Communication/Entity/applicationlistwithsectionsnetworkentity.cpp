#include "applicationlistwithsectionsnetworkentity.h"

ApplicationListWithSectionsNetworkEntity::ApplicationListWithSectionsNetworkEntity(QObject *parent)
{

}

QList<ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel> ApplicationListWithSectionsNetworkEntity::getSections() const {
    return this->_sectionModels;
}

void ApplicationListWithSectionsNetworkEntity::addSection(ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel sectionModel) {
    this->_sectionModels.append(sectionModel);
}

bool ApplicationListWithSectionsNetworkEntity::isEmpty() const {
    return this->_sectionModels.isEmpty();
}


// ---------------------
// --- section model ---
ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel::ApplicationListSectionModel(QString sectionTitle, QString sectionBackgroundImageUrl) :
    _sectionTitle(sectionTitle),
    _sectionBackgroundImageUrl(sectionBackgroundImageUrl)
{

}

void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel::addItemToSection(ApplicationBaseInfosNetworkEntityModel itemModel) {
    this->_itemsInSection.append(itemModel);
}

QList<ApplicationBaseInfosNetworkEntityModel> ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel::getItemsInSection() const {
    return this->_itemsInSection;
}

QString ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel::getSectionTitle() const {
    return this->_sectionTitle;
}

QString ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel::getSectionBackgroundImageUrl() const {
    return this->_sectionBackgroundImageUrl;
}

bool ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel::isEmpty() const {
    return this->_sectionTitle.isEmpty();
}

//// --------------------------
//// --- section item model ---
//ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::ApplicationListSectionItemModel() :
//    applicationID(""),
//    applicationName(""),
//    applicationIconURL(""),
//    applicationVersion(""),
//    applicationCategory(""),
//    applicationVersionSummedFlags(0),
//    applicationRatingCount(0),
//    applicationRating(0.0),
//    applicationReviewCount(0)
//{
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationID(QString applicationID) {
//    this->applicationID = applicationID;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationName(QString applicationName) {
//    this->applicationName = applicationName;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationIconURL(QString applicationIconURL) {
//    this->applicationIconURL = applicationIconURL;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationVersion(QString applicationVersion) {
//    this->applicationVersion = applicationVersion;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationCategory(QString applicationCategory) {
//    this->applicationCategory = applicationCategory;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationVersionSummedFlags(const int& applicationSummedFlags) {
//    this->applicationVersionSummedFlags = applicationSummedFlags;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationRatingCount(const int& applicationRatingCount) {
//    this->applicationRatingCount = applicationRatingCount;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationRating(const float& applicationRating) {
//    this->applicationRating = applicationRating;
//}

//void ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::setApplicationReviewCount(const int& applicationReviewCount) {
//    this->applicationReviewCount = applicationReviewCount;
//}

//QString ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationID() {
//    return applicationID;
//}

//QString ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationName() {
//    return applicationName;
//}

//QString ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationVersion() {
//    return applicationVersion;
//}

//QString ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationIconURL() {
//    return applicationIconURL;
//}

//QString ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationCategory() {
//    return applicationCategory;
//}

//int ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationVersionSummedFlags() {
//    return applicationVersionSummedFlags;
//}

//int ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationRatingCount() {
//    return applicationRatingCount;
//}

//float ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationRating() {
//    return applicationRating;
//}

//int ApplicationListWithSectionsNetworkEntity::ApplicationListSectionItemModel::getApplicationReviewCount() {
//    return applicationReviewCount;

//}
