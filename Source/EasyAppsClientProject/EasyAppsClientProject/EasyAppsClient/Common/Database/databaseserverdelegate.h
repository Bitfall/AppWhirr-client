#ifndef DATABASESERVERDELEGATE_H
#define DATABASESERVERDELEGATE_H

class DatabaseWorkPerformerBase;

class DatabaseServerDelegate {
public:
    //
    // When this method called you have to create and give a database-work-performer.
    //  * It will be managed by the Server, you SHOULD NOT delete it!
    //  * And generally you should not do anything with the given DatabaseWorkPerformer.
    virtual DatabaseWorkPerformerBase *createDatabaseWorkerWithDatabaseAtPath(QString databasePath) = 0;
};

#endif // DATABASESERVERDELEGATE_H
