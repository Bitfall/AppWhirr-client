#ifndef GOODBYEERRORLISTITEMWIDGET_H
#define GOODBYEERRORLISTITEMWIDGET_H

#include <QWidget>

class GoodbyeErrorListItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GoodbyeErrorListItemWidget(QString errorText, QString relatedFolderPath, QWidget *parent = 0);
    
Q_SIGNALS:
    void openThisFolder(QString folderPath);

private Q_SLOTS:
    void openThisFolderButtonClicked();

private:
    QString _errorText;
    QString _relatedFolderPath;
};

#endif // GOODBYEERRORLISTITEMWIDGET_H
