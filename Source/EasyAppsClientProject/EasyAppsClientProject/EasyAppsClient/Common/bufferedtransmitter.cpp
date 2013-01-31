#include "bufferedtransmitter.h"

namespace AW {
namespace Common {
namespace Utils {

BufferedTransmitter::BufferedTransmitter(qint32 bufferByteSize, QObject *parent) :
    QObject(parent),
    _bufferByteSize(bufferByteSize)
{
}

void BufferedTransmitter::bufferData(QByteArray data)
{
    _dataBuffer.append(data);

    if(_dataBuffer.size() >= _bufferByteSize) {
        this->forceFlushBuffer();
    }
}

void BufferedTransmitter::forceFlushBuffer()
{
    Q_EMIT bufferFlush(_dataBuffer);

    _dataBuffer.clear();
}

} // namespace Utils
} // namespace Common
} // namespace AW
