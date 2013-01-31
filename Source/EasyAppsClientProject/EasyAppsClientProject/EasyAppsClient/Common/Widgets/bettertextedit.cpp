#include "bettertextedit.h"

#include <QEvent>

BetterTextEdit::BetterTextEdit(QWidget *parent) :
    QTextEdit(parent),
    _isPresentPlaceholderText(true),
    _isNextTextChangeIsPlaceholder(false)
{
    connect(this, SIGNAL(textChanged()), this, SLOT(_textChanged()));
}

void BetterTextEdit::setPlaceholderText(QString placeholderText)
{
    _placeholderText = placeholderText;
    this->presentPlaceholderTextIfRequired();
}

bool BetterTextEdit::getIsPlaceholderText() const
{
    return _isPresentPlaceholderText;
}

QString BetterTextEdit::getNonPlaceholderText() const
{
    if(this->getIsPlaceholderText()) {
        return QString(); // empty string - the current text is the placeholder
    }

    return this->toPlainText();
}

bool BetterTextEdit::event(QEvent *e)
{
    if(e->type() == QEvent::FocusIn) {
        if(_isPresentPlaceholderText) {
            // remove the placeholder text
            this->setPlainText("");
        }
    }
    else if(e->type() == QEvent::FocusOut) {
        this->presentPlaceholderTextIfRequired();
    }

    return QTextEdit::event(e);
}

void BetterTextEdit::_textChanged()
{
//    DLog("Text changed: ") << this->toPlainText();
    if(_isNextTextChangeIsPlaceholder) {
        // it's a placeholder text, do nothing
        _isNextTextChangeIsPlaceholder = false;
        return;
    }

    if(!this->hasFocus() && this->toPlainText().isEmpty()) {
        DLog("Non focus widget.");
        this->presentPlaceholderTextIfRequired();
        return;
    }

    _isPresentPlaceholderText = false;
}

void BetterTextEdit::presentPlaceholderTextIfRequired()
{
    if(_isPresentPlaceholderText || this->toPlainText().isEmpty()) // placeholder required or it's an empty text
    {
        _isNextTextChangeIsPlaceholder = true;
        _isPresentPlaceholderText = true;
        this->setPlainText(_placeholderText);
    }
}
