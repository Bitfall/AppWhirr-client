#ifndef PLATFORMSPECIFICRUNNINGEXECUTABLEDATAINTERFACE_H
#define PLATFORMSPECIFICRUNNINGEXECUTABLEDATAINTERFACE_H

#include <QString>

//
// this is just a "marker class"
//  you have to inherit for the desired platform specific implementation
//  example: check the Win_RunningExecutableData class
class PlatformspecificRunningExecutableDataInterface {
public:
    PlatformspecificRunningExecutableDataInterface(QString relatedExecutablePath) : _relatedExecutablePath(relatedExecutablePath) {}
    virtual ~PlatformspecificRunningExecutableDataInterface() {}

    //
    // required for debugging (and required for the windows solution)
    QString getRelatedExecutablePath() const { return _relatedExecutablePath; }

private:
    QString _relatedExecutablePath;
};

#endif // PLATFORMSPECIFICRUNNINGEXECUTABLEDATAINTERFACE_H
