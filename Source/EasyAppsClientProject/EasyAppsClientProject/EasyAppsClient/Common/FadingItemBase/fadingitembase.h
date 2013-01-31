#ifndef FADINGITEMBASE_H
#define FADINGITEMBASE_H

//
// subclass of this should also has a property:
//  Q_PROPERTY(float fadeValue READ getFadeValue WRITE setFadeValue)
// but unfortunatelly C++ does not allow inheritance from 2 QObject s, and QObject is required for Q_PROPERTY
class FadingItemBase
{
public:
    FadingItemBase();
    virtual ~FadingItemBase() {}

    void setFadeValue(float fadeValue);
    float getFadeValue() const;

protected:
    virtual void fadeValueChanged() {}

private:
    float _fadeValue;
};

#endif // FADINGITEMBASE_H
