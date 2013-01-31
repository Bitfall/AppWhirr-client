#ifndef GOODBYEVIEW_H
#define GOODBYEVIEW_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

class GoodbyeView : public QWidget
{
    Q_OBJECT
public:
    explicit GoodbyeView(QWidget *parent = 0);

    QVBoxLayout *getErrorListLayout() const;
    void setErrorFoundText(QString value);
    
Q_SIGNALS:
    void quitFromUninstallerButtonClicked();

private:
    QVBoxLayout *_errorListLayout;
    QLabel *_errorFoundLabel;
};

#endif // GOODBYEVIEW_H
