#ifndef FADINGITEMINTERFACE_H
#define FADINGITEMINTERFACE_H

//
// to implement fade in/out in a widget easily use the FadingItemDelegate
class FadingItemInterface {
public:
    virtual void startFadeInAnimation() = 0;
    virtual void startFadeOutAnimation() = 0;
};

#endif // FADINGITEMINTERFACE_H
