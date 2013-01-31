#ifndef HELPITEMPRESENTERWINDOW_H
#define HELPITEMPRESENTERWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QWebView;
QT_END_NAMESPACE

class HelpItemPresenterWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit HelpItemPresenterWindow(QWidget *parent = 0);

    void setTitle(QString titleText);
    void setUrl(QString urlString);
    void setHtmlContent(QString content);

private:
    QLabel *_titleLabel;
    QWebView *_mainContentWebView;
};

#endif // HELPITEMPRESENTERWINDOW_H
