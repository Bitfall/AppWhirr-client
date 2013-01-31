#ifndef AWTASK_H
#define AWTASK_H

#include <QObject>
#include <QRunnable>

class AWTask : public QObject, public QRunnable
{
    Q_OBJECT
    
public:
    AWTask(QObject *parent = 0) :
        QObject(parent) {}

    virtual void run() = 0;
    
};

#endif // AWTASK_H
