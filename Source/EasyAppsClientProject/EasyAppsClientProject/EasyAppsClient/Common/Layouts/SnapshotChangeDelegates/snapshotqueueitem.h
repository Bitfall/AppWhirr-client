#ifndef SNAPSHOTQUEUEITEM_H
#define SNAPSHOTQUEUEITEM_H

#include "Common/Sorting/widgetlistsnapshot.h"

class SnapshotQueueItem
{
public:
    SnapshotQueueItem() {}

    SnapshotQueueItem(WidgetListSnapshot sourceSnapshot, WidgetListSnapshot targetSnapshot):
            sourceSnapshot(sourceSnapshot),
            targetSnapshot(targetSnapshot)
    {

    }

    WidgetListSnapshot getSourceSnapshot() const { return this->sourceSnapshot; }
    WidgetListSnapshot getTargetSnapshot() const { return this->targetSnapshot; }

    void setSourceSnapshot(WidgetListSnapshot snapshot) { this->sourceSnapshot = snapshot; }
    void setTargetSnapshot(WidgetListSnapshot snapshot) { this->targetSnapshot = snapshot; }

private:
    WidgetListSnapshot sourceSnapshot;
    WidgetListSnapshot targetSnapshot;
};

#endif // SNAPSHOTQUEUEITEM_H
