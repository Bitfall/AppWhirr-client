#ifndef URLBUTTON_H
#define URLBUTTON_H

#include <QPushButton>

class UrlButton : public QPushButton
{
    Q_OBJECT
public:
    explicit UrlButton(QWidget *parent = 0);
    explicit UrlButton(QString titleText, QWidget *parent = 0);
    explicit UrlButton(QString titleText, QString relatedUrl, QWidget *parent = 0);

    void setRelatedUrl(QString relatedUrl);
    QString getRelatedUrl() const;

Q_SIGNALS:
    void clickedWithUrl(QString relatedUrl);

private Q_SLOTS:
    void __clickedSlot();

private:
    void __commonInit();

private:
    QString _relatedUrl;
};

#endif // URLBUTTON_H
