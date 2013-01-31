#ifndef BETTERTEXTEDIT_H
#define BETTERTEXTEDIT_H

#include <QTextEdit>

class BetterTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit BetterTextEdit(QWidget *parent = 0);
    
    /*! Set a plain placeholder text
    */
    void setPlaceholderText(QString placeholderText);

    /*! Returns whether the current text is a placeholder text
    */
    bool getIsPlaceholderText() const;

    /*! Returns the non-placeholder text.

        If the current presented text is a placeholder it will return an empty string. In other case it will return the presented text.
    */
    QString getNonPlaceholderText() const;

protected:
    virtual bool event(QEvent *e);

private Q_SLOTS:
    void _textChanged();

private:
    void presentPlaceholderTextIfRequired();

private:
    QString _placeholderText;
    bool _isPresentPlaceholderText;
    bool _isNextTextChangeIsPlaceholder;
};

#endif // BETTERTEXTEDIT_H
