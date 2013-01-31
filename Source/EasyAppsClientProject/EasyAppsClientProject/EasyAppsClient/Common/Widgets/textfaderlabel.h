#ifndef TEXTFADERLABEL_H
#define TEXTFADERLABEL_H

#include <QLabel>
#include <QList>
#include <QTimer>
#include <QColor>
#include <QPaintEvent>

class TextFaderLabel : public QLabel
{
    Q_OBJECT

    Q_PROPERTY(double alpha READ getAlpha \
               WRITE setAlpha)

public:
    explicit TextFaderLabel(QWidget *parent = 0);
    ~TextFaderLabel();

    // Add new text to the list
    void addTextToShow(const QString& text);

    // Properties
    double getAlpha() const { return alpha; }
    //double alpha() { return alpha; }
    void setAlpha(double newAlpha) { alpha = newAlpha; this->repaint(); /*this->update();*/ }

    void paintEvent(QPaintEvent *e);
Q_SIGNALS:
    // Calls the show event
    void showEvent();
    // Fires timer manually
    void fireManually();

    // fading Q_SIGNALS
    void _fadeIn();
    void _fadeOut();
private Q_SLOTS:
    // Sets the new text as the shown text
    void showEventSlot();

    // fading Q_SLOTS
    void _didFadeOut();
    void _didFadeIn();
private:
    void _setupStateMachine();

    // Holds the upcomming infos
    QList<QString>              elementsToShow;
    // Fires in every 5 seconds
    QTimer*                     timer;
    double                      alpha;
    int                         duration;

};

#endif // TEXTFADERLABEL_H
