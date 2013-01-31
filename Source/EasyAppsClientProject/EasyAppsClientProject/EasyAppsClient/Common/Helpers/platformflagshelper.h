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
        Win_INVALID = 0,
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
        Mac_INVALID = 0,
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
        MacOS_10_7_2 = 16384,
        MacOS_10_7_3 = 32768,
        MacOS_10_7_4 = 65536
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

        QString getAsPrintableText()
        {
            QString printableText;
            if(this->getMainOsFlag() == PF_OS_Windows) {
                // main os version
                printableText = "Windows";
                // sub version
                switch(this->getSummedOsSubFlags()) {
                case Win_XP_NoSP:
                    printableText += " XP (no SP)";
                    break;
                case Win_XP_SP1:
                    printableText += " XP SP1";
                    break;
                case Win_XP_SP2:
                    printableText += " XP SP2";
                    break;
                case Win_XP_SP3:
                    printableText += " XP SP3";
                    break;
                case Win_Vista_NoSP:
                    printableText += " Vista (no SP)";
                    break;
                case Win_Vista_SP1:
                    printableText += " Vista SP1";
                    break;
                case Win_Vista_SP2:
                    printableText += " Vista SP2";
                    break;
                case Win_7_NoSP:
                    printableText += " 7 (no SP)";
                    break;
                case Win_7_SP1:
                    printableText += " 7 SP1";
                    break;
                case Win_8_NoSP:
                    printableText += " 8 (no SP)";
                    break;
                default:
                    printableText += " undefined version";
                }
            }
            else if(this->getMainOsFlag() == PF_OS_Mac) {
                // main os version
                printableText = "Mac";
                // sub version

                switch(this->getSummedOsSubFlags()) {
                case MacOS_10_6_0:
                    printableText += " OS X 10.6.0";
                    break;
                case MacOS_10_6_1:
                    printableText += " OS X 10.6.1";
                    break;
                case MacOS_10_6_2:
                    printableText += " OS X 10.6.2";
                    break;
                case MacOS_10_6_3:
                    printableText += " OS X 10.6.3";
                    break;
                case MacOS_10_6_4:
                    printableText += " OS X 10.6.4";
                    break;
                case MacOS_10_6_5:
                    printableText += " OS X 10.6.5";
                    break;
                case MacOS_10_6_6:
                    printableText += " OS X 10.6.6";
                    break;
                case MacOS_10_6_7:
                    printableText += " OS X 10.6.7";
                    break;
                case MacOS_10_6_8:
                    printableText += " OS X 10.6.8";
                    break;
                case MacOS_10_7_0:
                    printableText += " OS X 10.7.0";
                    break;
                case MacOS_10_7_1:
                    printableText += " OS X 10.7.1";
                    break;
                case MacOS_10_7_2:
                    printableText += " OS X 10.7.2";
                    break;
                case MacOS_10_7_3:
                    printableText += " OS X 10.7.3";
                    break;
                case MacOS_10_7_4:
                    printableText += " OS X 10.7.4";
                    break;
                default:
                    printableText += " undefined version";
                }
            }
            else {
                printableText = "Undefined OS";
            }

            // bitness
            if(this->getSummedBitnessFlags() == Bit32) {
                printableText += " (32bit)";
            }
            else if(this->getSummedBitnessFlags() == Bit64) {
                printableText += " (64bit)";
            }
            else {
                printableText += " (no bitness info)";
            }

            return printableText;
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

private:
    PlatformFlagsHelper();

public:

    //
    // compares the components of the given infos
    //  will return if founds match in ever component of the 2 infos
    //  example: info1 is a Win, Vista SP1 32 bit, info 2 is Win, Vista every version (no SP, SP1, SP2) and 32 and 64 bit
    //      will return true because both are Win, both has the Vista SP1 flag, and both has the 32 bit bitness flag
    //      but if info2 contains only 64 bitness flag then it will return false because the bitness is incompatible
    static bool isTheseInfosAreCompatible(SummedSinglePlatformInformations *info1, SummedSinglePlatformInformations *info2);

    // other utility methods
    static QString _convertIntBasedFlagsToStringBased(quint64 intBasedFlags);
};

#endif // PLATFORMFLAGSHELPER_H
