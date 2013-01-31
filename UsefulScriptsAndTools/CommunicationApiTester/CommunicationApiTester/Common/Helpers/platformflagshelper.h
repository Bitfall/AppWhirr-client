#ifndef PLATFORMFLAGSHELPER_H
#define PLATFORMFLAGSHELPER_H

#include <QString>

class PlatformFlagsHelper
{
public:
    enum PlatformFlagsOS {
        PF_OS_Windows = 1,
        PF_OS_Mac = 2
    };
    typedef quint32 SummedPlatformFlagsOS;

    enum WinSubFlags {
        // XP
        Win_XP_NoSP = 8,
        Win_XP_SP1 = 16,
        Win_XP_SP2 = 32,
        Win_XP_SP3 = 64,
        // Vista
        Win_Vista_NoSP = 128,
        Win_Vista_SP1 = 256,
        Win_Vista_SP2 = 512,
        // Win7
        Win_7_NoSP = 1024,
        Win_7_SP1 = 2048,
        // Win8
        Win_8_NoSP = 4096
    };
    typedef quint64 SummedWinSubFlags;

    enum MacSubFlags {
        // Snow Leo
        MacOS_10_6_0 = 8,
        MacOS_10_6_1 = 16,
        MacOS_10_6_2 = 32,
        MacOS_10_6_3 = 64,
        MacOS_10_6_4 = 128,
        MacOS_10_6_5 = 256,
        MacOS_10_6_6 = 512,
        MacOS_10_6_7 = 1024,
        MacOS_10_6_8 = 2048,
        // Lion
        MacOS_10_7_0 = 4096,
        MacOS_10_7_1 = 8192,
        MacOS_10_7_2 = 16384
    };
    typedef quint64 SummedMacSubFlags;

    enum PlatformFlagsBitness {
        Bit32 = 1,
        Bit64 = 2
    };
    typedef quint32 SummedBitnessFlags;

    //
    //
    class SummedSinglePlatformInformations {
    public:
        virtual PlatformFlagsOS getMainOsFlag() const = 0;
        virtual quint64 getSummedOsSubFlags() const = 0;
        virtual quint32 getSummedBitnessFlags() const = 0;
        //
        // combination of os-sub-flags and bitness
        quint64 _getSummedSubFlags() {
            return (this->getSummedBitnessFlags() | this->getSummedOsSubFlags());
        }
    };

    class SummedWinPlatformInformations : public SummedSinglePlatformInformations {
    public:
        explicit SummedWinPlatformInformations() : _summedWinPlatformFlags(0), _summedBitnessFlags(0) {}
        explicit SummedWinPlatformInformations(SummedWinSubFlags summedWinSubFlags, SummedBitnessFlags summedBitnessFlags) : _summedWinPlatformFlags(summedWinSubFlags), _summedBitnessFlags(summedBitnessFlags) {}

        PlatformFlagsOS getMainOsFlag() const { return PF_OS_Windows; }
        SummedWinSubFlags getSummedOsSubFlags() const { return _summedWinPlatformFlags; }
        SummedPlatformFlagsOS getSummedBitnessFlags() const { return _summedBitnessFlags; }

        //
        // factory method (utility)
        //  the summedSubFlags contains the bitness and the win-sub-flags as well
        static SummedWinPlatformInformations *_createBySummedSubFlags(quint64 summedSubFlags) {
            quint64 mask = 0x07; // the first 3 bits are the bitness flags
            quint32 bitnessFlags = summedSubFlags & mask;
            quint64 winSubFlags = summedSubFlags & (~mask);
            return new SummedWinPlatformInformations(winSubFlags, bitnessFlags);
        }
    private:
        SummedWinSubFlags _summedWinPlatformFlags;
        SummedBitnessFlags _summedBitnessFlags;
    };

    class SummedMacPlatformInformations : public SummedSinglePlatformInformations {
    public:
        explicit SummedMacPlatformInformations() : _summedMacPlatformFlags(0), _summedBitnessFlags(0) {}
        explicit SummedMacPlatformInformations(SummedMacSubFlags summedMacPlatformFlags, SummedBitnessFlags summedBitnessFlags) : _summedMacPlatformFlags(summedMacPlatformFlags), _summedBitnessFlags(summedBitnessFlags) {}

        PlatformFlagsOS getMainOsFlag() const { return PF_OS_Mac; }
        SummedMacSubFlags getSummedOsSubFlags() const { return _summedMacPlatformFlags; }
        SummedPlatformFlagsOS getSummedBitnessFlags() const { return _summedBitnessFlags; }

        //
        // factory method (utility)
        //  the summedSubFlags contains the bitness and the mac-sub-flags as well
        static SummedMacPlatformInformations *_createBySummedSubFlags(quint64 summedSubFlags) {
            quint64 mask = 0x07; // the first 3 bits are the bitness flags
            quint32 bitnessFlags = summedSubFlags & mask; // the first 3 bits are the bitness flags
            quint64 macSubFlags = summedSubFlags & (~mask);
            return new SummedMacPlatformInformations(macSubFlags, bitnessFlags);
        }
    private:
        SummedMacSubFlags _summedMacPlatformFlags;
        SummedBitnessFlags _summedBitnessFlags;
    };

    //
    // the purpose of this class is to provide a holder object for a platform-info object
    //  this object will manage it's platform-info object's memory automatically
    //  and can provide some utility methods as well
    //
    // HAVE TO BE MODIFIED TO USE REFERENCE-COUNTING
//    class SummedPlatformInformationsHolder {
//        explicit SummedPlatformInformationsHolder(SummedSinglePlatformInformations *relatedPlatformInfo) : _relatedPlatformInfo(relatedPlatformInfo) {}
//        explicit SummedPlatformInformationsHolder() : _relatedPlatformInfo(NULL) {}
//        ~SummedPlatformInformationsHolder() { if(_relatedPlatformInfo != NULL) { delete _relatedPlatformInfo; } }

//        SummedSinglePlatformInformations *getRelatedPlatformInfo() const { return _relatedPlatformInfo; }

//    private:
//        SummedSinglePlatformInformations *_relatedPlatformInfo;
//    };

    //
    // this is a collection for every supported platforms
//    class SummedPlatformFlagsCollection {
//        explicit SummedPlatformFlagsCollection(SummedWinPlatformInformations summedWinPlatformFlags, SummedMacPlatformInformations summedMacPlatformFlags) : _summedWinPlatformFlags(summedWinPlatformFlags), _summedMacPlatformFlags(summedMacPlatformFlags) {}

//        explicit SummedPlatformFlagsCollection(SummedWinPlatformInformations summedWinPlatformFlags) : _summedWinPlatformFlags(summedWinPlatformFlags) {}

//        explicit SummedPlatformFlagsCollection(SummedMacPlatformInformations summedMacPlatformFlags) : _summedMacPlatformFlags(summedMacPlatformFlags) {}

//    private:
//        SummedWinPlatformInformations _summedWinPlatformFlags;
//        SummedMacPlatformInformations _summedMacPlatformFlags;
//    };

private:
    PlatformFlagsHelper();

public:
//    static QString buildSummedPlatformFlagsByComponents(PlatformFlagsOS os, PlatformFlagsBitness bitness);
//    static bool isTheseFlagsAreCompatible(QString firstFlag, QString secondFlag);

    //
    // compares the components of the given infos
    //  will return if founds match in ever component of the 2 infos
    //  example: info1 is a Win, Vista SP1 32 bit, info 2 is Win, Vista every version (no SP, SP1, SP2) and 32 and 64 bit
    //      will return true because both are Win, both has the Vista SP1 flag, and both has the 32 bit bitness flag
    //      but if info2 contains only 64 bitness flag then it will return false because the bitness is incompatible
    static bool isTheseInfosAreCompatible(SummedSinglePlatformInformations *info1, SummedSinglePlatformInformations *info2);

//    static quint64 _buildSummedPlatformFlagsByComponentsAsInt(PlatformFlagsOS os, PlatformFlagsBitness bitness);
//    static bool _isTheseFlagsAreCompatibleIntVersion(quint64 firstFlagInt, quint64 secondFlagInt);

    // other utility methods
    static QString _convertIntBasedFlagsToStringBased(quint64 intBasedFlags);

public:
    static quint64 __getAllMacFlagsSummed();
    static quint64 __getAllWinFlagsSummed();
};

#endif // PLATFORMFLAGSHELPER_H
