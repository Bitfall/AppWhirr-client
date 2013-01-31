#ifndef POPUPWEBVIEWWIDGET_H
#define POPUPWEBVIEWWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QWebView;
QT_END_NAMESPACE

class PopupWebviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWebviewWidget(QWidget *parent = 0);

    void setHtmlContent(QString content);

public Q_SLOTS:
    void synchronizeGeometry(QRect targetGeometry);

private:
    QWebView *_mainContentWebView;
};

#endif // POPUPWEBVIEWWIDGET_H
