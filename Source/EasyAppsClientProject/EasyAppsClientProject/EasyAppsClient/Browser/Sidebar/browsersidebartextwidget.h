#ifndef BROWSERSIDEBARTEXTWIDGET_H
#define BROWSERSIDEBARTEXTWIDGET_H

#include "Common/Datasource/datasourceelementwidget.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class BrowserSideBarTextWidget : public DataSourceElementWidget
{
    Q_OBJECT
public:
    explicit BrowserSideBarTextWidget(const QString& title, QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:

private:
    QLabel*             label;
};

#endif // BROWSERSIDEBARTEXTWIDGET_H
