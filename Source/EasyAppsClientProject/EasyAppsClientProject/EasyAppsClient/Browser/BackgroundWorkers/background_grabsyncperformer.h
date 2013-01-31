#ifndef BACKGROUND_GRABSYNCPERFORMER_H
#define BACKGROUND_GRABSYNCPERFORMER_H

#include <QObject>
#include <QRunnable>

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
}
}
}


class Background_GrabSyncPerformer : public QObject, public QRunnable {
    Q_OBJECT
public:
    explicit Background_GrabSyncPerformer(QList<AW::Common::Communication::BaseNetworkEntity *> resultList);

    void run();

Q_SIGNALS:
    void finished();

protected:
    QList<AW::Common::Communication::BaseNetworkEntity *> _resultList;
};

#endif // BACKGROUND_GRABSYNCPERFORMER_H
