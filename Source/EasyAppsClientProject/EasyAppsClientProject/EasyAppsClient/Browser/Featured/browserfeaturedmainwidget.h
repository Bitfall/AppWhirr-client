#ifndef BROWSERFEATUREDMAINWIDGET_H
#define BROWSERFEATUREDMAINWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QImage;
class QTimer;
class QStateMachine;
QT_END_NAMESPACE

class BrowserFeaturedMainWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int imageShifted READ getImageShifted WRITE setImageShifted)
public:
    explicit BrowserFeaturedMainWidget(QWidget *parent = 0);
    ~BrowserFeaturedMainWidget();

Q_SIGNALS:
    void _shift();

protected Q_SLOTS:
    void changeFeaturedImage();
    void _didShift();

protected:
    void paintEvent(QPaintEvent* paintEvent);

private:
    void _setupStateMachine();

    QTimer*             timer;
    QStateMachine*      machine;

    void                setImageShifted(const int& shifted) { imageShifted = shifted; this->update(); }
    int                 getImageShifted() { return imageShifted; }
    int                 imageShifted;
};

#endif // BROWSERFEATUREDMAINWIDGET_H
