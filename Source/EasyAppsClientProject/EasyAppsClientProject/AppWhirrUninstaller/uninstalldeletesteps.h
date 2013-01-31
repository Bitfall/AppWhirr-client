#ifndef UNINSTALLDELETESTEPS_H
#define UNINSTALLDELETESTEPS_H

class UninstallDeleteSteps {
public:
    enum UninstallDeleteStepsEnum {
        None = 0,
        RemoveAppWhirrClientFiles = 1,  // the AppWhirr client app(s) - including the AppWhirrU app as well
        RemoveUserApps = 2,             // the local, user apps
        RemoveAppWhirrLocalFiles = 3    // every other local AppWhirr folder - except the user apps
    };
};

#endif // UNINSTALLDELETESTEPS_H
