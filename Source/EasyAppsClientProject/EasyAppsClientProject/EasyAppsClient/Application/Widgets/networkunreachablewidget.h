#ifndef NETWORKUNREACHABLEWIDGET_H
#define NETWORKUNREACHABLEWIDGET_H

#include "Common/Widgets/fadingwidgetbase.h"

class NetworkUnreachableWidget : public FadingWidgetBase
{
    Q_OBJECT
public:
    explicit NetworkUnreachableWidget(QWidget *parent = 0);

    // ---------------------------------------------------------------
    // --- FadingPreformerDelegateInterface implementation methods ---
    void hideContentForFadeAnimation();
    void showContentForFadeAnimation();
    void preserveCurrentSize();
    void dynamicSizeAllowed();

Q_SIGNALS:
    void retry();
    void ok();

protected:
    void paintEvent(QPaintEvent *e);

private:
    QWidget *_contentHolderWidget;
};

#endif // NETWORKUNREACHABLEWIDGET_H
