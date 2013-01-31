#ifndef HELPITEMWIDGET_H
#define HELPITEMWIDGET_H

#include <QWidget>
#include <QString>

namespace Ui {
    class HelpItemWidget;
}

class HelpItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HelpItemWidget(QWidget *parent = 0);
    ~HelpItemWidget();

    void setTitleText(QString titleText);
    void loadThisHtml(QString html);

private:
    Ui::HelpItemWidget *ui;
};

#endif // HELPITEMWIDGET_H
