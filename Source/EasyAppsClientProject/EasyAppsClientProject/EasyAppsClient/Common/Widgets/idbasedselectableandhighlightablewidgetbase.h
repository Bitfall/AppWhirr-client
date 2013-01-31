#ifndef IDBASEDSELECTABLEANDHIGHLIGHTABLEWIDGETBASE_H
#define IDBASEDSELECTABLEANDHIGHLIGHTABLEWIDGETBASE_H

#include "Common/Widgets/idbasedwidgetbase.h"

//
// you have to emit the SIGNALS if you want it to work correctly as a highlightable item
class IdBasedSelectableAndHighlightableWidgetBase : public IdBasedWidgetBase
{
    Q_OBJECT
public:
    explicit IdBasedSelectableAndHighlightableWidgetBase(QWidget *parent = 0);

    //
    virtual QString getId() const = 0;


    //
    virtual void startHighlighted() {}
    virtual void startNotHighlighted() {}

    virtual void startActivate() {}
    virtual void startDeactivate() {}

Q_SIGNALS:
    void mouseEntered(IdBasedSelectableAndHighlightableWidgetBase *source);
    void mouseLeaved(IdBasedSelectableAndHighlightableWidgetBase *source);
    //
    // emit it when this widget is activated. Activated is not the same as clicked - click can start an intermediate state (e.g. showing some informations), activated is a special option - it can be emited right after it's clicked, but it can be emitted after a confirmation as well (like: do you really want to activate?)
    void activated(IdBasedSelectableAndHighlightableWidgetBase *source);
    //
    void clicked(IdBasedSelectableAndHighlightableWidgetBase *source);

};

#endif // IDBASEDSELECTABLEANDHIGHLIGHTABLEWIDGETBASE_H
