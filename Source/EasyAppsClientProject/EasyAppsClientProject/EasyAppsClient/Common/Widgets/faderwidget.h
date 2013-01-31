#ifndef FADERWIDGET_H
#define FADERWIDGET_H

#include <QWidget>
#include <QPixmap>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class FaderWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float fadeOpacity READ getFadeOpacity WRITE setFadeOpacity)

private:
    enum FadeDirection
    {
        FD_In,
        FD_Out
    };

public:
    explicit FaderWidget(int fadeDuration, QWidget *parent = 0);
    ~FaderWidget();

    void fadeThisWidgetOut(QWidget *targetWidget);
    void fadeThisWidgetIn(QWidget *targetWidget);

Q_SIGNALS:
    void fadeFinish();

private Q_SLOTS:
    void _fadeAnimationFinished();


protected:
    void paintEvent(QPaintEvent *e);

private:
    float getFadeOpacity() const;
    void setFadeOpacity(float value);

    void _doFade(FadeDirection fadeDirection, float startValue, float endValue, int duration, QWidget *targetWidget);

private:
    QPixmap targetWidgetSnapshot;
    float fadeOpacity;
    QTimer *fadeRefreshTimer;
    int _fadeDuration;
};

#endif // FADERWIDGET_H
