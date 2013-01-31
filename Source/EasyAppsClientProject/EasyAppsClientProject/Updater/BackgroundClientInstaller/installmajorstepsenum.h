#ifndef INSTALLMAJORSTEPSENUM_H
#define INSTALLMAJORSTEPSENUM_H

enum InstallMajorSteps {
    DoFromTheBeginning = 0,
    BackupFinished = 1,
    OldVersionRemoved = 2,
    NewVersionCopiedToItsFinalPlace = 3
};

#endif // INSTALLMAJORSTEPSENUM_H
