#include "multicontentmanager.h"

#include <QLayout>

MultiContentManager::MultiContentManager(QLayout *contentHolderLayout, QObject *parent) :
    QObject(parent),
    _contentHolderLayout(contentHolderLayout)
{
}

MultiContentManager::~MultiContentManager()
{
    QMapIterator<QString, QWidget *> it(_contentWidgetMap);
    while(it.hasNext()) {
        it.next();

//        QWidget *relatedContentWidget = it.value();
//        AW_QOBJECT_SAFE_DELETE(relatedContentWidget);
        Q_EMIT contentItemCanBeReleased(it.key());
    }
}

void MultiContentManager::setContentWidgetToId(QString contentId, QWidget *contentWidget, bool isReplacePreviousContentForThisId)
{
    if(contentId.isEmpty()) {
        WLog("Invalid contentId (empty)");
        return;
    }

    if(contentWidget == NULL) {
        WLog("Invalid content widget (NULL).");
        return;
    }

    if(_contentWidgetMap.contains(contentId)) {
        // remove the previous one

        QWidget *relatedContentWidget = _contentWidgetMap.value(contentId);

        if(_currContentId == contentId) {
            if(!isReplacePreviousContentForThisId) {
                WLog("Content is already set for this ID, and 'isReplacePreviousContentForThisId' is false. Won't do anything.");
                return;
            }

            // it's the current one, remove it from layout
            // ! and do transition if required
            _contentHolderLayout->removeWidget(relatedContentWidget);
        }

        // release it
//        AW_QOBJECT_SAFE_DELETE(relatedContentWidget);
        Q_EMIT contentItemCanBeReleased(contentId);
    }

    // store the given content widget
    _contentWidgetMap[contentId] = contentWidget;
}

void MultiContentManager::switchContent(QString contentId)
{
    if(contentId == _currContentId) {
        DLog("This is the current content.");
        return;
    }

    // get the related target-widget
    QWidget *targetWidget = _contentWidgetMap.value(contentId, NULL);
    if(targetWidget == NULL) {
        WLog("Invalid contentId, no related content-widget found.");
        return;
    }

    // get and remove (from the layout) the current content-widget if any
    QWidget *currContentWidget = _contentWidgetMap.value(_currContentId, NULL);
    if(currContentWidget != NULL) {
        DLog("Current content widget found, will be removed.");
        _contentHolderLayout->removeWidget(currContentWidget);
        currContentWidget->hide();
    }
    else {
        DLog("No current content widget found.");
    }

    // add the target-widget to the layout
    _contentHolderLayout->addWidget(targetWidget);
    targetWidget->show();
    _currContentId = contentId;
}

QWidget *MultiContentManager::getContentWidget(QString contentId)
{
    return this->_contentWidgetMap.value(contentId, NULL);
}
