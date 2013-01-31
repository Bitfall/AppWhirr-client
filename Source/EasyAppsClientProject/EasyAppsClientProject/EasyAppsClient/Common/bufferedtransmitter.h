#ifndef BUFFEREDTRANSMITTER_H
#define BUFFEREDTRANSMITTER_H

#include <QObject>

namespace AW {
namespace Common {
namespace Utils {

class BufferedTransmitter : public QObject
{
    Q_OBJECT
public:
    /*! Constructor

        @param bufferByteSize : the byte-size of the buffer. \n
If greater-than-0: flush will happen every time there's more-or-equal data buffered. \n
If \a bufferByteSize is 0 then it won't buffer any data, it will flush it immediately after data added to the buffer. \n
If \a bufferByteSize is less-than-0: it will never flush the buffer (only when requested with \a forceFlushBuffer() )
*/
    explicit BufferedTransmitter(qint32 bufferByteSize, QObject *parent = 0);

public Q_SLOTS:
    void bufferData(QByteArray data);
    void forceFlushBuffer();

Q_SIGNALS:
    void bufferFlush(QByteArray bufferedData);
    
private:
    QByteArray _dataBuffer;
    qint32 _bufferByteSize;
};

} // namespace Utils
} // namespace Common
} // namespace AW

#endif // BUFFEREDTRANSMITTER_H
