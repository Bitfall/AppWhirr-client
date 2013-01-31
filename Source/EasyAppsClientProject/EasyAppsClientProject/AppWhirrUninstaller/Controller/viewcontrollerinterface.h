#ifndef VIEWCONTROLLERINTERFACE_H
#define VIEWCONTROLLERINTERFACE_H

#include <QObject>
#include <QWidget>

class ViewControllerInterface : public QObject
{
    Q_OBJECT
public:
    explicit ViewControllerInterface(QObject *parent = 0) :
        QObject(parent)
    {}

    virtual QWidget *getView() const = 0;
    virtual void destroyView() = 0;
};

#endif // VIEWCONTROLLERINTERFACE_H
