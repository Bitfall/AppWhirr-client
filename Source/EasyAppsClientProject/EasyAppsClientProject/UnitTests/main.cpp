#include <QTest>

#include "platformflagshelpertest.h"

int main(int argc, char *argv[])
{
    PlatformFlagsHelperTest *pfhTest = new PlatformFlagsHelperTest;
    QTest::qExec(pfhTest);
    delete pfhTest;

    return 0;
}
