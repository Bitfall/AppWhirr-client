#ifndef INTERPROCESSCOMMUNICATOR_H
#define INTERPROCESSCOMMUNICATOR_H

#include <QObject>

class InterProcessCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit InterProcessCommunicator(QObject *parent = 0);

};

#endif // INTERPROCESSCOMMUNICATOR_H
