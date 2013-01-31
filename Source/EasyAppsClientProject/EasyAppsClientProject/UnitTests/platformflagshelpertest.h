#ifndef PLATFORMFLAGSHELPERTEST_H
#define PLATFORMFLAGSHELPERTEST_H

#include <QObject>

class PlatformFlagsHelperTest : public QObject
{
    Q_OBJECT
public:
    explicit PlatformFlagsHelperTest(QObject *parent = 0);
    
private Q_SLOTS:
    void sameWinFlagsShouldBeCompatible_data();
    void sameWinFlagsShouldBeCompatible();

    void sameMacFlagsShouldBeCompatible_data();
    void sameMacFlagsShouldBeCompatible();

    void anInvalidFlagShouldNotBeCompatibleWithOthers();

    void noWinAndMacFlagsShouldBeCompatible_data();
    void noWinAndMacFlagsShouldBeCompatible();

    void distinctFlagsShouldNotBeCompatible_data();
    void distinctFlagsShouldNotBeCompatible();

    void intBasedFlagToStringBasedFlagShouldMatchASimpleConversion();

private:
    void prepareTestData_EveryWinFlags();
    void prepareTestData_EveryMacFlags();
    void prepareTestData_RandomMacAndWinFlags();
    void prepareTestData_RandomDistinctFlagCombinations();
    void prepareTestData_RandomOverlappingFlagCombinations();
};

#endif // PLATFORMFLAGSHELPERTEST_H
