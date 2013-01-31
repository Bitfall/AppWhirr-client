#ifndef APPICONANDBUTTONPRESENTERWIDGET_H
#define APPICONANDBUTTONPRESENTERWIDGET_H

#include <QString>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
QT_END_NAMESPACE

//#include "Common/FadingItemBase/fadingitembase.h"
#include "Common/fadingiteminterface.h"
#include "Common/fadingitemdelegate.h"

#include "Common/Widgets/idbasedselectableandhighlightablewidgetbase.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Common/Widgets/iconimagewidget.h"

class FadingButton;
class AppSubStatesPresenterWidget;

// ===========================
// Description:
//      The working mechanics:
//          - you set an image with setImage()
//          - this will load either a QImage or a QSvgRenderer initialized with the incoming image
//          - and a QPicture is used to present the data, and can handle both QImage and QSvgRenderer
//          - and finally, the QPicture is presented (drawn)
//          [!] The difference: a QImage is drawn directly to QPicture when setImage is called
//              but the QSvgRenderer will render in every paintEvent to the QPicture to allow free-scale of the svg image
//              This means drawing the svg image is probably much more processing heavy. But it allows free-scale.
// ===========================
class AppIconAndButtonPresenterWidget : public IdBasedSelectableAndHighlightableWidgetBase, public FadingItemInterface, FadingItemDelegate::FadingPerformerDelegateInterface
{
    Q_OBJECT
    Q_PROPERTY(float backgroundColorMultiplier READ getBackgroundColorMultiplier WRITE setBackgroundColorMultiplier)
    Q_PROPERTY(float highlightRatio READ getHighlightRatio WRITE setHighlightRatio)

public:
    explicit AppIconAndButtonPresenterWidget(QString labelStyleSheet, QWidget *parent = 0);

    void setContentByAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel);
    void refreshContentByNewAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);

#if 0
    void startToHide();
    void startToShow();
#endif


    // IdBasedSelectableAndHighlightableWidgetBase method implementations

    QString getId() const;
    void startHighlighted();
    void startNotHighlighted();

    void startActivate();
    void startDeactivate();

    void installEventFilterOnIconImage(QObject *filterTarget);


    //

//    ApplicationDescriptionModel getIconModel() const;
    QString getRelatedAppId() const;
    QString getRelatedAppName() const;
    IconImageWidget *getIconImageWidget() const;
    ApplicationDescriptionModel getRelatedAppDescriptionModel();

    // --------------------------------------------------
    // --- FadingItemInterface implementation methods ---
    void startFadeInAnimation();
    void startFadeOutAnimation();

    // ---------------------------------------------------------------
    // --- FadingPreformerDelegateInterface implementation methods ---
    void performNormalPaint(QPaintEvent *e);
    void performPaintOnlyThisPixmapWithOpacity(QPixmap pixmap, double requiredOpacity, QPaintEvent *e);
    QWidget *getPresenterWidget();

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *e);

    QSize sizeHint() const;

//    void fadeValueChanged();

Q_SIGNALS:
    void runOptionActivated(IdBasedSelectableAndHighlightableWidgetBase *source);
    void installOptionActivated(IdBasedSelectableAndHighlightableWidgetBase *source);

    void updateSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source);
    void notAvailableForThisPlatformSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source);
    void removedFromStoreSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source);

    void manageOptionActivated(IdBasedSelectableAndHighlightableWidgetBase *source);


    // ----------------
    // --- internal ---
Q_SIGNALS:

    // _ means: don't use them directly, only if you know what you do - rather you
    //      should use one of the wrapper functions, e.g.: grow will Q_EMIT _growSignal if it is allowed

    void _startHighlightedAnimationSignal();
    void _startBackFromHighlightedToNormalAnimationSignal();

    void _startActivatedAnimationSignal();
    void _startBackFromActivatedToDefaultAnimationSignal();


private Q_SLOTS:
    void _iconImageClickedSlot();
    void _mainAppStateDependentOptionActivated();
    void _manageAppButtonActivated();

    void activatedAnimationEnded();
    void backFromActivatedToDefaultAnimationEnded();

    void _updateSubStateIconActivated();
    void _notAvailableForThisPlatformSubStateIconActivated();
    void _removedFromStoreSubStateIconActivated();

private:
    void mouseEnteredEvent();
    void mouseLeavedEvent();

    void grow();
    void shrink();

    void setIconState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newState);

    void _setContent(QString appId, QString appName, QString iconFilePath, ImageTypes::ImageTypesEnum imageType, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState, AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags appSubStates);

    float getBackgroundColorMultiplier() const;
    void setBackgroundColorMultiplier(float value);

    float getHighlightRatio() const;
    void setHighlightRatio(float value);

private:
    ApplicationDescriptionModel _relatedAppDescriptionModel;
//    QString _relatedAppId;

    //
    // widgets
    IconImageWidget *iconImageWidget;
    QLabel *titleLabel;
    FadingButton *_runButton;
    FadingButton *_manageAppButton;
    AppSubStatesPresenterWidget *_appSubStatePresenterWidget;

    FadingItemDelegate *_fadingItemDelegate;
//    float _fadeValue;

    float _backgroundColorMultiplier;
    float _highlightRatio;
};

#endif // APPICONANDBUTTONPRESENTERWIDGET_H
