#ifndef APPLICATIONSTOREAPPLICATIONPAGE_H
#define APPLICATIONSTOREAPPLICATIONPAGE_H

#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QString;
class QPixmap;
class QPaintEvent;
class QLabel;
class QPushButton;
class QScrollArea;
class QWebView;
QT_END_NAMESPACE

#include "Browser/Content/Page/browserpagebase.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

class MenuItemWidget;
class ApplicationDetailInfosNetworkEntity;
class SimpleImagePresenterWidget;
class RatingWidget;
class MultiContentManager;
class UrlButton;
class PopupWebviewWidget;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
class ResourceDownloadStreamToMemory;
}
}
}

class ApplicationStoreApplicationPage : public BrowserPageWidgetBase
{
    Q_OBJECT

private:
    class ThumbnailItemModel {
    public:
        explicit ThumbnailItemModel() : _isImage(true) {}
        explicit ThumbnailItemModel(QString url, QPixmap thumbnailPixmap, bool isImage) : _url(url), _thumbnailPixmap(thumbnailPixmap), _isImage(isImage) {}

        QString getUrl() const { return _url; }
        QPixmap getThumbnailPixmap() const { return _thumbnailPixmap; }
        bool getIsImage() const { return _isImage; }

    private:
        QString _url;
        QPixmap _thumbnailPixmap;
        bool _isImage;
    };

public:
    explicit ApplicationStoreApplicationPage(QString pageId, ApplicationDescriptionModel appDescriptionModel, QWidget *parent = 0);

    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

protected Q_SLOTS:
    // Communication
    void networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities);
    void failedToGetDescription(const QString& error);
    void networkResourceRequestsFinished();
    void retrievedResourceForURL(QUrl URL, QByteArray resource);

    // Menu
    void relatedMenuItemActivated(QString menuItemId);

    // Button
    void mainAppStateDependentButtonClicked();
    void morePressed(QString url);

    // Screenshot
    void thumbnailSelected(QString url, QPixmap thumbnailPixmap, bool image);
    void fullSizedImageLoaded(QUrl URL, QByteArray resource);

protected:
    QList<QString> getSubpageRequestURLs();

private Q_SLOTS:
    //
    // will be called when the page request a getAppState
//    void _getAppStateResult(QString appId, AppWhirr::ApplicationStates::ApplicationStatesEnum newAppState, QString operationId, bool isQuerySuccess);
    void _getAppDescriptionResult(ApplicationDescriptionModel resultAppDescription, QString dbOperationId, bool isQuerySuccess);
    //
    // will be called every time an app-state changes successfully
    void _appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    //
    // will be called every time a bacth app-state change happens successfully
    void _batchAppStateChanged(QMap<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> appIdAndNewStateMap);
    //
    void _wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel);

    void _backButtonPressed();
    void _showLicenceButtonPressed();



    void _openExternalLinkUrl(QString url);

private:
    void refreshByRelatedAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    void refreshByRelatedAppDescription(ApplicationDescriptionModel newAppDescriptionModel);

    void _runCurrentApp();
    QString _getCurrAppName();
    QString _getCurrAppId();
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum _getCurrAppState();

private:
    QVBoxLayout*                        mainLayout;
    QHBoxLayout*                        appDetailContentHolderLayout;
    QVBoxLayout*                        contentLayout;
//    QString                             applicationID;
//    QString                             appName;
    QString                             fullDescription;

    // media
    QVBoxLayout *_relatedFullSizeMediaContainerLayout;
    QWidget *_currFullSizeMediaPresenterWidget;
//    SimpleImagePresenterWidget*         fullSizedImage;
//    QWebView*                           fullSizedVideo;
    QHBoxLayout*                        thumbnailLayout;
    QWidget*                            thumbnailContainer;
    QMap<QString, QByteArray>           loadedScreenshots;

    QLabel*                             title;
    QLabel*                             description;
    QLabel*                             moreLabel;

    // -- sidebar
    QVBoxLayout                         *_sidebarLayout;
    SimpleImagePresenterWidget          *_appIconImagePresenter;
    QPushButton                         *_mainAppStateDependentButton;
    QLabel                           *_devWebsiteLinkLabel;
    QLabel                           *_supportLinkLabel;
    // app info
    QLabel *_appInfoTitleLabel;
    QLabel *_appInfoCategoryLabel;
    QLabel *_appInfoVersionTextLabel;
    QLabel *_appInfoVersionLabel;
    QLabel *_appInfoPriceLabel;
    // requirements
    QLabel *_appRequirementsTitleLabel;
    QLabel *_appRequirementsWindowsLabel;
    QLabel *_appRequirementsMacLabel;
    QLabel *_appRequirementsHddTextLabel;
    QLabel *_appRequirementsHddLabel;

    // --
    QHBoxLayout*                        titleLayout;
//    RatingWidget*                       ratingWidget;

//    QWebView*                           twitterView;

    // -- internal
    ApplicationDetailInfosNetworkEntity*           appEntity;
    QScrollArea* thumbnailScrollArea;

    ApplicationDescriptionModel _appDescriptionModel;

    bool _isContentLoaded;

    MultiContentManager *_pageContentManager;

    ThumbnailItemModel _lastSelectedThumbnailInfo;

    AW::Common::Communication::ResourceDownloadStreamToMemory *_resourceDownloadStreamToMemory;
};

#endif // APPLICATIONSTOREAPPLICATIONPAGE_H
