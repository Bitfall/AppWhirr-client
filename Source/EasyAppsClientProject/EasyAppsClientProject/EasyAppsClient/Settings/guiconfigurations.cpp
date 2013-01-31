#include "Settings/guiconfigurations.h"

GuiConfigurations::GuiConfigurations()
{
}


// -------------------------
// --- Color definitions ---

//
// greens

//// default dark
//const QString GuiConfigurations::defaultDarkGreenColorHex() {
//    return "#6bb130";
//}

//const QString GuiConfigurations::defaultDarkGreenHighlightedColorHex() {
//    return "#519400";
//}

//// default light
//const QString GuiConfigurations::defaultLightGreenColorHex() {
//    return "#71c52b";
//}

//const QString GuiConfigurations::defaultLightGreenHighlightedColorhex() {
//    return "#68b227";
//}

////
//// grays

//const QString GuiConfigurations::defaultLightGrayColorHex() {
//    return "#bebebe";
//}

//const QString GuiConfigurations::defaultLightGrayHighlightedColorHex() {
//    return "#a3a3a3";
//}

//const QString GuiConfigurations::defaultLightGrayGradientStartColorHex() {
//    return "#cecece";
//}

//const QString GuiConfigurations::defaultDarkGrayColorHex() {
//    return "#666666";
//}

//const QString GuiConfigurations::defaultDarkGrayHighlightedColorHex() {
//    return "#333333";
//}

//const QString GuiConfigurations::defaultLightGrayBackgroundColorHex() {
//    return "#f0f0f0";
//}

//const QString GuiConfigurations::defaultLightGrayWidgetBackgroundColorHex() {
//    return "#f0f0f0";
//}

//const QString GuiConfigurations::defaultLightGrayHighlightedWidgetBackgroundColorHex() {
//    return "#e6e6e6";
//}

// ---------------------------
// Launcher


//
// widget sizes

const QSize GuiConfigurations::applicationIconPresenterWithButtonWidgetFixedSize()
{
    QSize r(200, 200);
    return r;
}

const QSize GuiConfigurations::applicationIconImageWidgetShrinkedSize()
{
    QSize r(90, 90);
    return r;
}

const QSize GuiConfigurations::applicationIconImageWidgetGrownSize()
{
    QSize r(100, 100);
    return r;
}

//const QSize GuiConfigurations::compactLauncherWindowFixedSize()
//{
//    QSize r(1000, 378);
//    return r;
//}

//const QSize GuiConfigurations::casualLauncherWindowFixedSize()
//{
//    QSize r(850, 578);
//    return r;
//}

//const QSize GuiConfigurations::contentScrollAreaSize()
//{
//    QSize r(compactLauncherWindowFixedSize().width() - 100, 200);
//    return r;
//}

const QSize GuiConfigurations::zeroSize()
{
    QSize r(0, 0);
    return r;
}

//const QString GuiConfigurations::casualLauncherIconPresenterLabelStyleSheet()
//{
//    QString r("*{color:#2e2e2e;}");
//    return r;
//}

//const QString GuiConfigurations::compactLauncherIconPresenterLabelStyleSheet()
//{
//    QString r("*{color:#dedede;}");
//    return r;
//}

//const QString GuiConfigurations::headerMinimizeButtonStyleSheet()
//{
//    QString r("QPushButton {"
//                     "border-width: 1px;"
//                     "border-image: url(:/headerbar/MinimizeButton-Normal) 1 1 1 1 stretch stretch;"
//                     "}"

//                     "QPushButton:hover {"
//                     "border-width: 1px;"
//                     "border-image: url(:/headerbar/MinimizeButton-Hover) 1 1 1 1 stretch stretch;"
//                     "}"
//                     );

//    return r;
//}

//const QString GuiConfigurations::headerMaximizeButtonStyleSheet()
//{
//    QString r("QPushButton {"
//                     "border-width: 1px;"
//                     "border-image: url(:/headerbar/MaximizeButton-Normal) 1 1 1 1 stretch stretch;"
//                     "}"

//                     "QPushButton:hover {"
//                     "border-width: 1px;"
//                     "border-image: url(:/headerbar/MaximizeButton-Hover) 1 1 1 1 stretch stretch;"
//                     "}"
//                     );

//    return r;
//}

//const QString GuiConfigurations::headerCloseButtonStyleSheet()
//{
//    QString r("QPushButton {"
//                     "border-width: 1px;"
//                     "border-image: url(:/headerbar/CloseButton-Normal) 1 1 1 1 stretch stretch;"
//                     "}"

//                     "QPushButton:hover {"
//                     "border-width: 1px;"
//                     "border-image: url(:/headerbar/CloseButton-Hover) 1 1 1 1 stretch stretch;"
//                     "}"
//                     );

//    return r;
//}

// ---------------
// --- buttons ---

//const QString GuiConfigurations::_defaultButtonBaseStyleSheet()
//{
//    QString r("QPushButton {"
//                "border-style: solid;"
//                "border-width: 1px;"
//                "border-radius: 3px;"
//                "border-color: %1;"

//                "padding: 5px;"

//                "font: 14px arial, sans-serif;"
//              "}"

//              "QPushButton:hover {"
//                "border-color: %2;"
////                "border-width: 2px;"
//              "}"
//              );

//    return r.arg(defaultDarkGrayColorHex()).arg(defaultDarkGrayHighlightedColorHex());
//}

//const QString GuiConfigurations::highPriorityGreenButtonStyleSheet()
//{
//    QString r = defaultGreenButtonStyleSheet();

//    r += "QPushButton { font: bold 16px arial, sans-serif; }";

//    return r;
//}

//const QString GuiConfigurations::defaultGreenButtonStyleSheet()
//{
//    // with background (border) image
////    QString r("QPushButton {"
////                     "border-width: 5px;"
////                     "border-image: url(:/DefaultButton-Normal) 5 5 5 5 stretch stretch;"
////                     "font: 14px arial, sans-serif;"
////                     "color: #ffffff;"
////                     "}"

////              "QPushButton:hover {"
////                     "border-width: 5px;"
////                     "border-image: url(:/DefaultButton-Highlighted) 5 5 5 5 stretch stretch;"
////                     "}"

////              "QPushButton:!enabled {"
////                     "color: #cccccc;"
////                     "}"
////                     );

//    QString r = _defaultButtonBaseStyleSheet();

//    r += "QPushButton {"
//            "border-color: #4a8600;"

//            "background-color: %1;"

//            "font: 14px arial, sans-serif;"
//            "color: %3;"
//          "}"

//          "QPushButton:hover {"
////            "background-color: %2;"
//            "color: #ffffff;"
//            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %2, stop:0.1 %1, stop:0.9 %1, stop:1 %2);"
////            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:0.5 #5eaa00, stop:1 #58a000);"
//            "border-color: %2;"
//          "}"

//          "QPushButton:!enabled {"
//            "background-color: #%2;"
////            "color: #cccccc;"
//          "}";

//    return r.arg(defaultDarkGreenColorHex()).arg(defaultDarkGreenHighlightedColorHex()).arg(defaultLightGrayBackgroundColorHex());
//}

//const QString GuiConfigurations::lowPriorityGreyButtonStyleSheet()
//{
//    QString r = _defaultButtonBaseStyleSheet();

//    r += "QPushButton {"
//            "border-color: %1;"

//            "background-color: %3;"

//            "font: 14px arial, sans-serif;"
//            "color: %4;"
//          "}"

//          "QPushButton:hover {"
//            "color: %5;"
//            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %6, stop:0.1 %3, stop:0.9 %3, stop:1 %6);"

//            "border-color: %2;"
//          "}"

//          "QPushButton:!enabled {"
//            "background-color: %2;"
////            "color: %4;"
//          "}";

//    return r.arg(defaultLightGrayColorHex()).arg(defaultLightGrayHighlightedColorHex()).arg(defaultLightGrayWidgetBackgroundColorHex()).arg(defaultDarkGrayColorHex()).arg(defaultDarkGrayHighlightedColorHex()).arg(defaultLightGrayGradientStartColorHex());
//}

//const QString GuiConfigurations::appRunButtonStyleSheet_Enabled()
//{
//    return defaultGreenButtonStyleSheet();
//}

//const QSize GuiConfigurations::appRunButtonMinimumSize()
//{
//    QSize r(60, 30);
//    return r;
//}


// ------------------------
// --- checkbox-buttons ---

//const QString GuiConfigurations::defaultGreyCheckboxButtonStyleSheet()
//{
//    QString r = lowPriorityGreyButtonStyleSheet();
//    r = r.replace("QPushButton", "CheckboxButton");

//    r += "QLabel {"
//                "font: 14px arial, sans-serif;"
//                "color: %1;"
//            "}"

////            "CheckboxButton:hover QLabel {"
////                "font: 14px arial, sans-serif;"
////                "color: #ff6666;"
////            "}"

//            "QCheckBox {"
//                "spacing: 5px;"
//            "}"

//            "QCheckBox::indicator {"
//                "width: 20px;"
//                "height: 20px;"
//            "}"

//            "QCheckBox::indicator:unchecked {"
//                "image: url(:/images/checkbox/Checkbox-Unchecked-Normal);"
//            "}"

//            "QCheckBox::indicator:checked {"
//                "image: url(:/images/checkbox/Checkbox-Checked-Normal);"
//            "}";

//    return r.arg(defaultDarkGrayHighlightedColorHex());
//}

// ---------------------
// --- progress bars ---

//const QString GuiConfigurations::defaultProgressBarStyleSheet()
//{
//    QString r("QProgressBar {"
//                     "border: 1px solid #000000;"
//                     "border-radius: 0px;"
//                     "background-color: #549900;"
//                     "text-align: center;"
//                     "font:20px arial, sans-serif;"
//                     "color: #ffffff;"
//                     "}"

//              "QProgressBar::chunk {"
//                     "background-color: %1;"
//                     "width: 5px;"
//                     "}"

//              );

//    return r.arg(defaultLightGreenColorHex());
//}


// -------------
// --- texts ---

//const QString GuiConfigurations::defaultDarkTextLabelStyleSheet()
//{
//    QString r(
//                "QLabel {"
//                    "font: 14px arial, sans-serif;"
//                    "color: %1;"
//                    "padding: 5px;"
//                "}"

//                );

//    return r.arg(defaultDarkGrayHighlightedColorHex());
//}

//const QString GuiConfigurations::largeDarkTextLabelStyleSheet()
//{
//    QString r(
//                "QLabel {"
//                    "font: 18px arial, sans-serif;"
//                    "color: %1;"
//                    "padding: 5px;"
//                "}"

//                );

//    return r.arg(defaultDarkGrayHighlightedColorHex());
//}

//const QString GuiConfigurations::defaultWhiteTextLabelStyleSheet()
//{
//    QString r(
//                "QLabel {"
//                    "font: 14px arial, sans-serif;"
//                    "color: #ffffff;"
//                    "padding: 5px;"
//                "}"

//                );

//    return r;
//}

//const QString GuiConfigurations::largeWhiteTextLabelStyleSheet()
//{
//    QString r(
//                "QLabel {"
//                    "font: 18px arial, sans-serif;"
//                    "color: #ffffff;"
//                    "padding: 5px;"
//                "}"

//                );

//    return r;
//}

//
// menu

//const QString GuiConfigurations::menuInactiveTextLabelStyleSheet()
//{
//    QString r(
//                "QLabel {"
//                    "font: 16px arial, sans-serif;"
//                    "color: %1;"
////                    "padding: 5px;"
//                "}"

//                );

//    return r.arg(defaultDarkGrayColorHex());
//}

//const QString GuiConfigurations::menuActiveTextLabelStyleSheet()
//{
//    QString r(
//                "QLabel {"
//                    "font: 16px arial, sans-serif;"
//                    "color: %1;"
////                    "padding: 5px;"
//                "}"

//                );

//    return r.arg(defaultDarkGrayHighlightedColorHex());
//}


// ------------------
// --- line edits ---

//const QString GuiConfigurations::defaultLineEditStyleSheet()
//{

//    QString r("QLineEdit {"
//                "border-style: solid;"
//                "border-width: 1px;"
//                "border-radius: 2px;"
//                "border-color: %1;"

//                "padding: 5px;"

//                "font: 12px arial, sans-serif;"
//                "color: %1;"
//                "selection-background-color: %4;"

//                "background-color: %3;"
//              "}"

//              "QLineEdit:hover {"
//                "border-color: %2;"
//              "}"
//              );

//    return r.arg(defaultDarkGrayColorHex()).arg(defaultDarkGrayHighlightedColorHex()).arg(defaultLightGrayWidgetBackgroundColorHex()).arg(defaultLightGrayHighlightedWidgetBackgroundColorHex());
//}

//// search

//const QString GuiConfigurations::searchLineEditStyleSheet()
//{

//    QString r("QLineEdit {"
//                "border-style: solid;"
//                "border-width: 1px;"
//                "border-radius: 2px;"
//                "border-color: %1;"

//                "padding: 5px;"

//                "font: 12px arial, sans-serif;"
//                "color: %1;"
//                "selection-background-color: %4;"

//                "background-color: %3;"
//              "}"

//              "QLineEdit:hover {"
//                "border-color: %2;"
//              "}"
//              );

//    return r.arg(defaultDarkGrayColorHex()).arg(defaultDarkGrayHighlightedColorHex()).arg(defaultLightGrayWidgetBackgroundColorHex()).arg(defaultLightGrayHighlightedWidgetBackgroundColorHex());
//}

// ----------------


//const QString GuiConfigurations::casualLauncherContentScrollAreaStyleSheet()
//{
//    QString r("* { background: transparent; } "

//                     "QScrollBar:horizontal { background: #4f4f4f; } "
//                     "QScrollBar:left-arrow:horizontal, QScrollBar::right-arrow:horizontal {"
//                          "width: 0px;"
//                          "height: 0px;"
//                          "background: #8f8f8f; }"
//                     "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: #3f3f3f; }"

//                     "QScrollBar:vertical { background: #4f4f4f; } "
//                     "QScrollBar:left-arrow:vertical, QScrollBar::right-arrow:vertical {"
//                          "width: 0px;"
//                          "height: 0px;"
//                          "background: #8f8f8f; }"
//                     "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: #3f3f3f; }"

//                     );
//    return r;
//}

//const QString GuiConfigurations::compactLauncherContentScrollAreaStyleSheet()
//{
//    QString r("* { background: transparent; } "
//                     "QScrollBar:horizontal { background: #4f4f4f; } "
//                     "QScrollBar:left-arrow:horizontal, QScrollBar::right-arrow:horizontal {"
//                          "width: 0px;"
//                          "height: 0px;"
//                          "background: #8f8f8f; }"
//                     "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: #3f3f3f; }");
//    return r;
//}


//
// animation durations

int GuiConfigurations::iconImageWidgetRotateAnimationDuration()
{
    int r = 400;
    return r;
}

int GuiConfigurations::iconImageWidgetGrowShrinkAnimationDuration()
{
    int r = 250;
    return r;
}


//
// mouse calibrations
float GuiConfigurations::mouseGrabScrollSpeed()
{
    float r = 2.0f;
    return r;
}

int GuiConfigurations::mouseGrabScrollClickTreshold()
{
    int r = 5;
    return r;
}

//
float GuiConfigurations::grabScrollDropScrollScale()
{
    float r = 0.3f;
    return r;
}

int GuiConfigurations::grabScrollAdjustmentInterval()
{
    int r = 400;
    return r;
}

// ---------------------------
// Browser

//const QSize GuiConfigurations::browserWindowDefaultSize()
//{
//    QSize r(1000, 618);  // aranymetszes;)
//    return r;
//}

//const QSize GuiConfigurations::browserWindowMinimumSize()
//{
//    QSize r(1000, 618);
//    return r;
//}

//const int GuiConfigurations::mainMenuHolderSidebarExpandedFixedWidth() {
//    return 180;
//}

//const int GuiConfigurations::mainMenuFixedWidth() {
//    return 175;
//}

//int GuiConfigurations::mainPagePickerShrinkedHeight()
//{
//    int r = 30;
//    return r;
//}

//int GuiConfigurations::mainPagePickerGrownHeight()
//{
//    int r = 50;
//    return r;
//}

//int GuiConfigurations::mainPagePickerGrowShrinkAnimationDuration()
//{
//    int r = 300;
//    return r;
//}



//const QColor GuiConfigurations::menuButtonActiveBackgroundColor()
//{
//    QColor r(190, 190, 190);
//    return r;
//}

//const QColor GuiConfigurations::menuButtonPassiveBackgroundColor()
//{
//    QColor r(220, 220, 220);
//    return r;
//}

//const QColor GuiConfigurations::menuButtonActiveMarginColor()
//{
//    QColor r(255, 255, 255);
//    return r;
//}

//const QColor GuiConfigurations::menuButtonPassiveMarginColor()
//{
//    QColor r(0, 0, 0);
//    return r;
//}

//const QPoint GuiConfigurations::menuButtonRoundness()
//{
//    QPoint r(7, 7);
//    return r;
//}

//int GuiConfigurations::menuButtonSelectDeselectAnimationDuration()
//{
//    int r = 400;
//    return r;
//}

//const QMargins GuiConfigurations::menuButtonContentMargins()
//{
//    QMargins r(5, 2, 5, 2);
//    return r;
//}

//int GuiConfigurations::browserWindowNotificationWidgetHeight()
//{
//    int r = 30;
//    return r;
//}

//int GuiConfigurations::browserWindowNotificationAreaHeight()
//{
//    int r = 3 * 30;
//    return r;
//}

//const QString GuiConfigurations::browserDefaultWidgetStyleSheet()
//{
//    QString r("QWidget { background:#ff0000; }");
////    QString r("QWidget { background:#f0f0f0; }");
//    return r;
//}

//QString GuiConfigurations::dynamicScrollAreaStyleSheet()
//{
//    QString r(
//                "QWidget {"
//                "background: transparent"
//                "}"
//                );

//    return r + browserScrollerStyleSheet();
//}

//QString GuiConfigurations::browserScrollerStyleSheet() {
//    QString r(
//            "QScrollBar:vertical { background: #EDEDED; width: 15px; }"
//            "QScrollBar:handle:vertical { background: #AAAAAA; border-radius: 2px; margin: 0 2px 0 2px; }"
//            "QScrollBar:handle:vertical:hover { background: #6bb130; }"
//            "QScrollBar:add-line:vertical { background: #EDEDED; }"
//            "QScrollBar:sub-line:vertical { background: #EDEDED; }"
//            "QScrollBar:horizontal { background: #EDEDED; height: 15px; }"
//            "QScrollBar:handle:horizontal { background: #AAAAAA; border-radius: 2px; margin: 2px 0 2px 0; }"
//            "QScrollBar:handle:horizontal:hover { background: #6bb130; }"
//            "QScrollBar:add-line:horizontal  { background: #EDEDED; }"
//            "QScrollBar:sub-line:horizontal  { background: #EDEDED; }");

//    return r;
//}


// ---------------------------
// Common


//int GuiConfigurations::defaultSnapshotChangeShowHideAnimationDuration()
//{
//    int r = 200;
//    return r;
//}

//int GuiConfigurations::defaultSnapshotChangeMoveAnimationDuration()
//{
//    int r = 300;
//    return r;
//}

//int GuiConfigurations::browserNotificationSnapshotChangeShowHideAnimationDuration()
//{
//    int r = 300;
//    return r;
//}

//int GuiConfigurations::browserNotificationSnapshotChangeMoveAnimationDuration()
//{
//    int r = 500;
//    return r;
//}

//const QString GuiConfigurations::browserNotificationAreaStyleSheet(const QString &notificationWidgetCssId)
//{
//    QString s1("*[cssId=\"");
////    QString s2("\"] {background:#737373;}");
//    QString s2("\"] {background:#4c4c4c;}");
//    QString r = s1 + notificationWidgetCssId + s2;
//    return r;
//}

//int GuiConfigurations::browserNotificationAreaSizeChangeAnimationDuration()
//{
//    int r = 700;
//    return r;
//}

//int GuiConfigurations::browserNotificationAreaSpacingBetweenItems() {
//    int r = 5;
//    return r;
//}

//const QMargins GuiConfigurations::browserNotificationAreaContentMargins()
//{
//    QMargins r(1, 1, 1, 1);
//    return r;
//}

//QString GuiConfigurations::defaultNotificationWidgetStyleSheet()
//{
//    QString r(
//                "QWidget { "
//                    "background-color: #e6e6e6;"
//                "}"

//                );

//    r += defaultDarkTextLabelStyleSheet();

//    return r;
//}

//QString GuiConfigurations::simpleTextNotificationStyleSheetForPriorityLow() {
//    QString r("QWidget { background-color:#f0f0f0; }");
//    return r + defaultDarkTextLabelStyleSheet();
//}

//QString GuiConfigurations::simpleTextNotificationStyleSheetForPriorityNormal() {
//    QString r("QWidget { background-color:%1; }");
//    return r.arg(defaultLightGreenColorHex()) + defaultDarkTextLabelStyleSheet();
//}

//QString GuiConfigurations::simpleTextNotificationStyleSheetForPriorityHigh() {
//    QString r("QWidget { background-color:#6bb130; }");
//    return r + defaultWhiteTextLabelStyleSheet();
//}

//QString GuiConfigurations::simpleTextNotificationStyleSheetForPriorityCritical() {
//    QString r("QWidget { background-color:rgb(250, 220, 220); }");
//    return r + defaultDarkTextLabelStyleSheet();
//}

//QString GuiConfigurations::installProgressNotificationStyleSheetForPriorityLow() {
//    QString r("QWidget { background-color:#f0f0f0; }");
//    return r + defaultDarkTextLabelStyleSheet();
//}

//QString GuiConfigurations::installProgressNotificationStyleSheetForPriorityNormal() {
//    QString r("QWidget { background-color:%1; }");
//    return r.arg(defaultLightGreenColorHex()) + defaultDarkTextLabelStyleSheet();
//}

//QString GuiConfigurations::installProgressNotificationStyleSheetForPriorityHigh() {
//    QString r("QWidget { background-color:#6bb130; }");
//    return r + defaultWhiteTextLabelStyleSheet();
//}

//QString GuiConfigurations::installProgressNotificationStyleSheetForPriorityCritical() {
//    QString r("QWidget { background-color:rgb(250, 220, 220); }");
//    return r + defaultDarkTextLabelStyleSheet();
//}
