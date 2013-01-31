#ifndef SECTIONWITHFEATUREDITEMICONBACKGROUNDWIDGET_H
#define SECTIONWITHFEATUREDITEMICONBACKGROUNDWIDGET_H

#include <QWidget>

class SectionWithFeaturedItemIconBackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SectionWithFeaturedItemIconBackgroundWidget(QWidget *parent = 0);

    //
    // currently the pixmap will be drawn to 256x256 size
    void setBackgroundIconFromResource(QByteArray pixmapData);
    void setBackgroundIconPixmap(QPixmap pixmap);
protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap _backgroundIconPixmap;
};

#endif // SECTIONWITHFEATUREDITEMICONBACKGROUNDWIDGET_H
