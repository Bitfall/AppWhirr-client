#ifndef MULTICONTENTMANAGER_H
#define MULTICONTENTMANAGER_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QMap>

class MultiContentManager : public QObject
{
    Q_OBJECT
public:
    /*!
        @param contentHolderLayout : the layout in a page, window, or in any widget which will hold the dynamic content of the page/window/widget, managed by this manager. Generally: this layout should be used as the actual content - it's a 'placeholder' for the actual content. The actual content will be added to this layout.
    */
    explicit MultiContentManager(QLayout *contentHolderLayout, QObject *parent = 0);
    ~MultiContentManager();

    /*! Sets the \a contentWidget for the given \a contentId

        This method can be used for both adding a new \a contentId and for replacing an existing contentId's \a contentWidget.

        @param isReplacePreviousContentForThisId : if true then it will replace the previous content-widget set for this \a contentId , if found. You should probably manually test whether the item is already added before you set a new item for an existing contentId because of memory management practices.

        @note Memory management:
        The given \a contentWidget 's memory WON'T be managed by this manager.\n
    */
    void setContentWidgetToId(QString contentId, QWidget *contentWidget, bool isReplacePreviousContentForThisId = false);

    /*! Switches from the current \a contentWidget to the one related to the \a contentId

        If \a contentId is not found it will do nothing.

        @note Switching:
        The current content will be set to hidden and will be removed from the
    */
    void switchContent(QString contentId);

    /*!
        @return The related content-widget, or NULL if the \a contentId is not found.
    */
    QWidget *getContentWidget(QString contentId);

Q_SIGNALS:
    void contentItemCanBeReleased(QString contentId);

private:
    QLayout *_contentHolderLayout;
    QMap<QString, QWidget *> _contentWidgetMap;
    QString _currContentId;
};

#endif // MULTICONTENTMANAGER_H
