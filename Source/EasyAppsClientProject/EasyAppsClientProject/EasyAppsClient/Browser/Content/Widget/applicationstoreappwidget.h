#ifndef APPLICATIONSTOREAPPWIDGET_H
#define APPLICATIONSTOREAPPWIDGET_H

#include <QWidget>

#include "Common/Widgets/idbasedselectableandhighlightablewidgetbase.h"
#include <QString>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPaintEvent;
class QMouseEvent;
QT_END_NAMESPACE

#include "Application/Communication/Entity/applicationbaseinfosnetworkentitymodel.h"

class FadingButton;

class ApplicationStoreAppWidget : public IdBasedSelectableAndHighlightableWidgetBase
{
    Q_OBJECT

    Q_PROPERTY(float highlightRatio READ getHighlightRatio WRITE setHighlightRatio)

private:
    enum AppGrabTypeEnum {
        AGT_NotGrabbed = 0,
        AGT_Grabbed = 1,
        AGT_GrabbedAndInstalled = 2
    };

public:
    explicit ApplicationStoreAppWidget(ApplicationBaseInfosNetworkEntityModel applicationListEntity, QWidget *parent = 0);
    explicit ApplicationStoreAppWidget(ApplicationDescriptionModel relatedAppDescriptionModel, QString category, QString iconUrl, QWidget *parent = 0);

    QString getId() const; // returns the app-name
    QString getRelatedAppName() const;
    QString getRelatedAppId() const;
    ApplicationDescriptionModel getRelatedAppDescription() const;
//    ApplicationListNetworkEntity* getEntity();
    //
    // if isForceRefresh is false then it will first check the previous app-state and refresh only if it's not the same
    void refresh(ApplicationDescriptionModel newAppDescription);
    void refreshByState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState, bool isForceRefresh);

Q_SIGNALS:
    void installOrUpdateThisApp(ApplicationDescriptionModel appDescriptionModel);
    void runThisApp(ApplicationDescriptionModel appDescriptionModel);

public Q_SLOTS:
    void retrievedResourceForURL(const QString& URL, const QByteArray& bytes);

protected:
    void paintEvent(QPaintEvent* paintEvent);
    void mouseReleaseEvent (QMouseEvent* event);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

private:
    void _refreshByCurrAppDescription();

    void __initialize(ApplicationDescriptionModel relatedAppDescriptionModel, QString category, QString iconUrl);

    float getHighlightRatio() const;
    void setHighlightRatio(float value);

    void _refreshGrabbedIndicatorIcon(AppGrabTypeEnum appGrabType);

Q_SIGNALS: // internal signals
    void __startHighlightedAnimationSignal();
    void __startBackFromHighlightedToNormalAnimationSignal();

private Q_SLOTS:
    void _mainAppStateDependentOptionActivated();
    void _iconImageLoaded(QImage image);

private:

//    ApplicationListNetworkEntity*           applicationListEntity;
    QPixmap                                 _iconPixmap;
    QPixmap _grabbedIndicatorIconPixmap;
//    QPixmap                                 _backgroundPixmap;
    QColor _backgroundColor;
    QColor _highlightedBackgroundColor;
    QColor _borderColor;
    QColor _highlightedBorderColor;

//    QString _appId;
//    QString _appName;
    QString _appCategory;
    QString _appIconUrl;

    ApplicationDescriptionModel _appDescrModel;

    float _highlightRatio;

    FadingButton *_mainOptionButton;

    AppGrabTypeEnum _currAppGrabType;
};

#endif // APPLICATIONSTOREAPPWIDGET_H
