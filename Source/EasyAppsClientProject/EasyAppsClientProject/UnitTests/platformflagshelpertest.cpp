#include "platformflagshelpertest.h"

#include "../EasyAppsClient/Common/Helpers/platformflagshelper.h"
#include <QTest>

PlatformFlagsHelperTest::PlatformFlagsHelperTest(QObject *parent) :
    QObject(parent)
{
}


void PlatformFlagsHelperTest::sameWinFlagsShouldBeCompatible_data()
{
    this->prepareTestData_EveryWinFlags();
}

void PlatformFlagsHelperTest::sameWinFlagsShouldBeCompatible()
{
    QFETCH(quint64, winSummedSubFlag);

    // 32bit
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi1 = new PlatformFlagsHelper::SummedWinPlatformInformations(winSummedSubFlag, PlatformFlagsHelper::Bit32);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi1, spi1), true );
    delete spi1;
    // 64bit
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi2 = new PlatformFlagsHelper::SummedWinPlatformInformations(winSummedSubFlag, PlatformFlagsHelper::Bit64);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi2, spi2), true );
    delete spi2;
}


void PlatformFlagsHelperTest::sameMacFlagsShouldBeCompatible_data()
{
    this->prepareTestData_EveryMacFlags();
}

void PlatformFlagsHelperTest::sameMacFlagsShouldBeCompatible()
{
    QFETCH(quint64, macSummedSubFlag);

    // 32bit
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi1 = new PlatformFlagsHelper::SummedMacPlatformInformations(macSummedSubFlag, PlatformFlagsHelper::Bit32);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi1, spi1), true );
    delete spi1;
    // 64bit
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi2 = new PlatformFlagsHelper::SummedMacPlatformInformations(macSummedSubFlag, PlatformFlagsHelper::Bit64);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi2, spi2), true );
    delete spi2;
}

void PlatformFlagsHelperTest::anInvalidFlagShouldNotBeCompatibleWithOthers()
{
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi11 = new PlatformFlagsHelper::SummedMacPlatformInformations(PlatformFlagsHelper::Win_INVALID, PlatformFlagsHelper::Bit32);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi12 = new PlatformFlagsHelper::SummedMacPlatformInformations(PlatformFlagsHelper::Win_XP_SP1, PlatformFlagsHelper::Bit32);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi11, spi12), false );
    delete spi11; delete spi12;

    PlatformFlagsHelper::SummedSinglePlatformInformations *spi21 = new PlatformFlagsHelper::SummedMacPlatformInformations(PlatformFlagsHelper::Win_INVALID, PlatformFlagsHelper::Bit32);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi22 = new PlatformFlagsHelper::SummedMacPlatformInformations(PlatformFlagsHelper::Win_INVALID, PlatformFlagsHelper::Bit32);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi21, spi22), false );
    delete spi21; delete spi22;

    PlatformFlagsHelper::SummedSinglePlatformInformations *spi31 = new PlatformFlagsHelper::SummedMacPlatformInformations(PlatformFlagsHelper::Win_XP_SP2, PlatformFlagsHelper::Bit32);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi32 = new PlatformFlagsHelper::SummedMacPlatformInformations(PlatformFlagsHelper::Win_INVALID, PlatformFlagsHelper::Bit32);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi31, spi32), false );
    delete spi31; delete spi32;
}

void PlatformFlagsHelperTest::noWinAndMacFlagsShouldBeCompatible_data()
{
    this->prepareTestData_RandomMacAndWinFlags();
}

void PlatformFlagsHelperTest::noWinAndMacFlagsShouldBeCompatible()
{
    QFETCH(quint64, macSummedSubFlag);
    QFETCH(quint64, winSummedSubFlag);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi1 = new PlatformFlagsHelper::SummedMacPlatformInformations(macSummedSubFlag, PlatformFlagsHelper::Bit32);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi2 = new PlatformFlagsHelper::SummedWinPlatformInformations(winSummedSubFlag, PlatformFlagsHelper::Bit32);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi1, spi2), false );
    delete spi1; delete spi2;
}


void PlatformFlagsHelperTest::distinctFlagsShouldNotBeCompatible_data()
{
    this->prepareTestData_RandomDistinctFlagCombinations();
}

void PlatformFlagsHelperTest::distinctFlagsShouldNotBeCompatible()
{
    QFETCH(quint64, summedOsSubFlag1);
    QFETCH(quint32, bitness1);
    QFETCH(quint64, summedOsSubFlag2);
    QFETCH(quint32, bitness2);

    // mac-mac
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi1 = new PlatformFlagsHelper::SummedMacPlatformInformations(summedOsSubFlag1, bitness1);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi2 = new PlatformFlagsHelper::SummedMacPlatformInformations(summedOsSubFlag2, bitness2);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi1, spi2), false );
    delete spi1; delete spi2;

    // win-win
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi21 = new PlatformFlagsHelper::SummedWinPlatformInformations(summedOsSubFlag1, bitness1);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi22 = new PlatformFlagsHelper::SummedWinPlatformInformations(summedOsSubFlag2, bitness2);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi21, spi22), false );
    delete spi21; delete spi22;

    // mac-win
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi31 = new PlatformFlagsHelper::SummedMacPlatformInformations(summedOsSubFlag1, bitness1);
    PlatformFlagsHelper::SummedSinglePlatformInformations *spi32 = new PlatformFlagsHelper::SummedWinPlatformInformations(summedOsSubFlag2, bitness2);
    QCOMPARE( PlatformFlagsHelper::isTheseInfosAreCompatible(spi31, spi32), false );
    delete spi31; delete spi32;
}


void PlatformFlagsHelperTest::intBasedFlagToStringBasedFlagShouldMatchASimpleConversion()
{
    quint64 intFlag = 63;
    QString stringFlag = PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(intFlag);
    QCOMPARE(stringFlag, QString("63"));
}

//
// private

void PlatformFlagsHelperTest::prepareTestData_EveryWinFlags()
{
    QTest::addColumn<quint64>("winSummedSubFlag");

    QTest::newRow("Win_XP_NoSP") << (quint64)PlatformFlagsHelper::Win_XP_NoSP;
    QTest::newRow("Win_XP_SP1") << (quint64)PlatformFlagsHelper::Win_XP_SP1;
    QTest::newRow("Win_XP_SP2") << (quint64)PlatformFlagsHelper::Win_XP_SP2;
    QTest::newRow("Win_XP_SP3") << (quint64)PlatformFlagsHelper::Win_XP_SP3;

    QTest::newRow("Win_Vista_NoSP") << (quint64)PlatformFlagsHelper::Win_Vista_NoSP;
    QTest::newRow("Win_Vista_SP1") << (quint64)PlatformFlagsHelper::Win_Vista_SP1;
    QTest::newRow("Win_Vista_SP2") << (quint64)PlatformFlagsHelper::Win_Vista_SP2;

    QTest::newRow("Win_7_NoSP") << (quint64)PlatformFlagsHelper::Win_7_NoSP;
    QTest::newRow("Win_7_SP1") << (quint64)PlatformFlagsHelper::Win_7_SP1;
    QTest::newRow("Win_8_NoSP") << (quint64)PlatformFlagsHelper::Win_8_NoSP;
}

void PlatformFlagsHelperTest::prepareTestData_EveryMacFlags()
{
    QTest::addColumn<quint64>("macSummedSubFlag");

    QTest::newRow("MacOS_10_6_0") << (quint64)PlatformFlagsHelper::MacOS_10_6_0;
    QTest::newRow("MacOS_10_6_1") << (quint64)PlatformFlagsHelper::MacOS_10_6_1;
    QTest::newRow("MacOS_10_6_2") << (quint64)PlatformFlagsHelper::MacOS_10_6_2;
    QTest::newRow("MacOS_10_6_3") << (quint64)PlatformFlagsHelper::MacOS_10_6_3;
    QTest::newRow("MacOS_10_6_4") << (quint64)PlatformFlagsHelper::MacOS_10_6_4;
    QTest::newRow("MacOS_10_6_5") << (quint64)PlatformFlagsHelper::MacOS_10_6_5;
    QTest::newRow("MacOS_10_6_6") << (quint64)PlatformFlagsHelper::MacOS_10_6_6;
    QTest::newRow("MacOS_10_6_7") << (quint64)PlatformFlagsHelper::MacOS_10_6_7;
    QTest::newRow("MacOS_10_6_8") << (quint64)PlatformFlagsHelper::MacOS_10_6_8;

    QTest::newRow("MacOS_10_7_0") << (quint64)PlatformFlagsHelper::MacOS_10_7_0;
    QTest::newRow("MacOS_10_7_1") << (quint64)PlatformFlagsHelper::MacOS_10_7_1;
    QTest::newRow("MacOS_10_7_2") << (quint64)PlatformFlagsHelper::MacOS_10_7_2;
    QTest::newRow("MacOS_10_7_3") << (quint64)PlatformFlagsHelper::MacOS_10_7_3;
    QTest::newRow("MacOS_10_7_4") << (quint64)PlatformFlagsHelper::MacOS_10_7_4;
}

void PlatformFlagsHelperTest::prepareTestData_RandomMacAndWinFlags()
{
    QTest::addColumn<quint64>("winSummedSubFlag");
    QTest::addColumn<quint64>("macSummedSubFlag");

    QTest::newRow("Random 1") << (quint64)PlatformFlagsHelper::Win_XP_NoSP << (quint64)PlatformFlagsHelper::MacOS_10_6_0;
    QTest::newRow("Random 2") << (quint64)PlatformFlagsHelper::Win_XP_SP2 << (quint64)PlatformFlagsHelper::MacOS_10_6_1;
    QTest::newRow("Random 3") << (quint64)PlatformFlagsHelper::Win_7_SP1 << (quint64)PlatformFlagsHelper::MacOS_10_6_5;
    QTest::newRow("Random 4") << (quint64)PlatformFlagsHelper::Win_8_NoSP << (quint64)PlatformFlagsHelper::MacOS_10_6_7;
    QTest::newRow("Random 5") << (quint64)PlatformFlagsHelper::Win_7_SP1 << (quint64)PlatformFlagsHelper::MacOS_10_7_2;
    QTest::newRow("Random 6") << (quint64)PlatformFlagsHelper::Win_XP_SP1 << (quint64)PlatformFlagsHelper::MacOS_10_7_3;
    QTest::newRow("Random 7") << (quint64)PlatformFlagsHelper::Win_7_NoSP << (quint64)PlatformFlagsHelper::MacOS_10_6_3;
    QTest::newRow("Random 8") << (quint64)PlatformFlagsHelper::Win_Vista_SP2 << (quint64)PlatformFlagsHelper::MacOS_10_7_2;
    QTest::newRow("Random 9") << (quint64)PlatformFlagsHelper::Win_XP_SP3 << (quint64)PlatformFlagsHelper::MacOS_10_7_3;
    QTest::newRow("Random 10") << (quint64)PlatformFlagsHelper::Win_7_SP1 << (quint64)PlatformFlagsHelper::MacOS_10_7_0;
}

void PlatformFlagsHelperTest::prepareTestData_RandomDistinctFlagCombinations()
{
    QTest::addColumn<quint64>("summedOsSubFlag1");
    QTest::addColumn<quint32>("bitness1");
    QTest::addColumn<quint64>("summedOsSubFlag2");
    QTest::addColumn<quint32>("bitness2");

    QTest::newRow("Random 1") << (quint64)24 << (quint32)PlatformFlagsHelper::Bit32
                              << (quint64)32 << (quint32)PlatformFlagsHelper::Bit32;
    QTest::newRow("Random 2") << (quint64)24 << (quint32)PlatformFlagsHelper::Bit32
                              << (quint64)24 << (quint32)PlatformFlagsHelper::Bit64;
}

void PlatformFlagsHelperTest::prepareTestData_RandomOverlappingFlagCombinations()
{
    QTest::addColumn<quint64>("summedOsSubFlag1");
    QTest::addColumn<quint32>("bitness1");
    QTest::addColumn<quint64>("summedOsSubFlag2");
    QTest::addColumn<quint32>("bitness2");

    QTest::newRow("Random 1") << (quint64)24 << (quint32)PlatformFlagsHelper::Bit32
                              << (quint64)8 << (quint32)PlatformFlagsHelper::Bit32;
    QTest::newRow("Random 1") << (quint64)24 << (quint32)PlatformFlagsHelper::Bit64
                              << (quint64)8 << (quint32)PlatformFlagsHelper::Bit64;
    QTest::newRow("Random 2") << (quint64)24 << (quint32)PlatformFlagsHelper::Bit32
                              << (quint64)16 << (quint32)PlatformFlagsHelper::Bit32;
    QTest::newRow("Random 3") << (quint64)24 << (quint32)PlatformFlagsHelper::Bit64
                              << (quint64)16 << (quint32)PlatformFlagsHelper::Bit64;
}
