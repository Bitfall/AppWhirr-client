#ifndef GUICONFIGURATIONS_H
#define GUICONFIGURATIONS_H

#include <QSize>
#include <QColor>
#include <QPoint>
#include <QMargins>


//
// NOTES:
//  --- colors ---
//  * HIGHLIGHTED: the highlighted colors are usually the same color, but 20% less bright (e.g. base color #f0f0f0 (94% brightness) -> highlighted: #bdbdbd (74% brightness), or 20% more bright (if the normal color is dark - e.g.: base color: #000000 -> highlighted: #333333)
//  ** or 10% brighter / less bright: #e5e5e5 / #1a1a1a
//
//  * default (dark) #GREEN color of controls (like buttons, progress-bars, ...): #6bb130 -> highlighted: #519400
//  ** with a default text color of full white #ffffff
//  * default (dark-dark) #green color of disabled controls: #549900
//  * default (lighter) #GREEN color: #71c52b
//
//  * default dark #GRAY: #737373
//  * even darker #gray (backgrounds): #4c4c4c / #767676
//  * light gray: #aaaaaa (about the gray equivalent of the default green #6bb130)
//  * default screen and #widget background: #f0f0f0
//  * default a bit highlighted area background gray: #e6e6e6
//
//  * #TEXT colors:
//  ** for bright backgrounds the default text color is full black #000000
//  *** and the disabled is 20% more bright: #333333 (20%)
//  ** for dark backgrounds the default text color is full white #ffffff
//  *** and the disabled is 20% less bright: #cccccc (80%)
//
//  --- font ---
//  * default font is Arial, regular (non bold, non italic, ...)
//  * every text should be Sans-serif (http://www.w3schools.com/css/css_font.asp)
//  ** font example (font:bold 26px arial, sans-serif;)
//  ** don't use italic font as well
//
class GuiConfigurations
{
private:
    GuiConfigurations();

public:

    // -------------------------
    // --- Color definitions ---

    // greens
//    static const QString defaultDarkGreenColorHex();
//    static const QString defaultDarkGreenHighlightedColorHex();
//    static const QString defaultLightGreenColorHex();
//    static const QString defaultLightGreenHighlightedColorhex();
//    // grays
//    static const QString defaultLightGrayColorHex();
//    static const QString defaultLightGrayHighlightedColorHex();
//    static const QString defaultLightGrayGradientStartColorHex();
//    static const QString defaultDarkGrayColorHex();
//    static const QString defaultDarkGrayHighlightedColorHex();
//    static const QString defaultLightGrayBackgroundColorHex();

//    static const QString defaultLightGrayWidgetBackgroundColorHex();
//    static const QString defaultLightGrayHighlightedWidgetBackgroundColorHex();

    // ------------------
    // Launcher

    //
    // widget sizes
    static const QSize applicationIconPresenterWithButtonWidgetFixedSize();

    static const QSize applicationIconImageWidgetShrinkedSize();
    static const QSize applicationIconImageWidgetGrownSize();

//    static const QSize compactLauncherWindowFixedSize();
//    static const QSize contentScrollAreaSize();

//    static const QSize casualLauncherWindowFixedSize();

    // utility sizes
    static const QSize zeroSize();

    //
    // icon-presenter animation durations
    static int iconImageWidgetRotateAnimationDuration();
    static int iconImageWidgetGrowShrinkAnimationDuration();

    //
    // mouse calibrations
    static float mouseGrabScrollSpeed();
    static int mouseGrabScrollClickTreshold();

    //
    static float grabScrollDropScrollScale();
    static int grabScrollAdjustmentInterval();

    //
    // style sheets
//    static const QString casualLauncherIconPresenterLabelStyleSheet();
//    static const QString casualLauncherContentScrollAreaStyleSheet();

//    static const QString compactLauncherIconPresenterLabelStyleSheet();
//    static const QString compactLauncherContentScrollAreaStyleSheet();


    // ------------------
    // Browser

//    static const QSize browserWindowDefaultSize();
//    static const QSize browserWindowMinimumSize();

//    static const int mainMenuHolderSidebarExpandedFixedWidth();
//    static const int mainMenuFixedWidth();

//    static const QString browserDefaultWidgetStyleSheet();

//    static int mainPagePickerShrinkedHeight();
//    static int mainPagePickerGrownHeight();
//    static int mainPagePickerGrowShrinkAnimationDuration();

//    static QString dynamicScrollAreaStyleSheet();
//    static QString browserScrollerStyleSheet();

//    static const QColor menuButtonActiveBackgroundColor();
//    static const QColor menuButtonPassiveBackgroundColor();
//    static const QColor menuButtonActiveMarginColor();
//    static const QColor menuButtonPassiveMarginColor();
//    static const QPoint menuButtonRoundness();
//    static int menuButtonSelectDeselectAnimationDuration();
//    static const QMargins menuButtonContentMargins();

    // notification widget
//    static int browserWindowNotificationWidgetHeight();
//    static int browserWindowNotificationAreaHeight();

    // notification area
//    static int browserNotificationSnapshotChangeShowHideAnimationDuration();
//    static int browserNotificationSnapshotChangeMoveAnimationDuration();
//    static const QString browserNotificationAreaStyleSheet(const QString &notificationWidgetCssId);
//    static int browserNotificationAreaSizeChangeAnimationDuration();
//    static int browserNotificationAreaSpacingBetweenItems();
//    static const QMargins browserNotificationAreaContentMargins();

//    static QString defaultNotificationWidgetStyleSheet();

//    static QString simpleTextNotificationStyleSheetForPriorityLow();
//    static QString simpleTextNotificationStyleSheetForPriorityNormal();
//    static QString simpleTextNotificationStyleSheetForPriorityHigh();
//    static QString simpleTextNotificationStyleSheetForPriorityCritical();

//    static QString installProgressNotificationStyleSheetForPriorityLow();
//    static QString installProgressNotificationStyleSheetForPriorityNormal();
//    static QString installProgressNotificationStyleSheetForPriorityHigh();
//    static QString installProgressNotificationStyleSheetForPriorityCritical();

    // ---------------
    // --- buttons ---

    //
    // defines a base button style
    //  * with border style, radius, color and width
    //  * with padding
    //  * with font: arial 14px, sans-serif
    //
    // so in most cases every other button derives from this and only the correct font (size) and (content and background) colors have to be set
    //
//    static const QString _defaultButtonBaseStyleSheet();
    //
//    static const QString defaultGreenButtonStyleSheet();
//    static const QString highPriorityGreenButtonStyleSheet();
//    static const QString lowPriorityGreyButtonStyleSheet();

    //
    // special buttons

    // app - run button
//    static const QString appRunButtonStyleSheet_Enabled();
//    static const QSize appRunButtonMinimumSize();

    // header-bar window-state buttons
//    static const QString headerMinimizeButtonStyleSheet();
//    static const QString headerMaximizeButtonStyleSheet();
//    static const QString headerCloseButtonStyleSheet();

    // ------------------------
    // --- checkbox-buttons ---
//    static const QString defaultGreyCheckboxButtonStyleSheet();

    // ---------------------
    // --- progress bars ---
//    static const QString defaultProgressBarStyleSheet();

    // -------------
    // --- texts ---

//    static const QString defaultDarkTextLabelStyleSheet();
//    static const QString largeDarkTextLabelStyleSheet();

//    static const QString defaultWhiteTextLabelStyleSheet();
//    static const QString largeWhiteTextLabelStyleSheet();

    // menu
//    static const QString menuInactiveTextLabelStyleSheet();
//    static const QString menuActiveTextLabelStyleSheet();

    // ------------------
    // --- line edits ---

//    static const QString defaultLineEditStyleSheet();

    // search
//    static const QString searchLineEditStyleSheet();

    // ------------------
    // Common

//    static int defaultSnapshotChangeShowHideAnimationDuration();
//    static int defaultSnapshotChangeMoveAnimationDuration();
};

#endif // GUICONFIGURATIONS_H
