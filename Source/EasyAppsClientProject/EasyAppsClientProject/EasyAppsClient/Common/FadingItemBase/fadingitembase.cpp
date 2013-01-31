#include "fadingitembase.h"

FadingItemBase::FadingItemBase() :
    _fadeValue(0)
{

}

void FadingItemBase::setFadeValue(float fadeValue) {
    this->_fadeValue = fadeValue;
}

float FadingItemBase::getFadeValue() const {
    return this->_fadeValue;
}
