#ifndef ADDNEWITEM_H
#define ADDNEWITEM_H

#include <QMainWindow>
#include "../Database/websitelistdatabaseitemmodel.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
QT_END_NAMESPACE

class AddOrEditItemDialog : public QMainWindow
{
    Q_OBJECT
public:
    explicit AddOrEditItemDialog(QWidget *parent = 0);
    explicit AddOrEditItemDialog(WebsiteListDatabaseItemModel editThisItem, QWidget *parent = 0);

Q_SIGNALS:
    void newItemAdded(WebsiteListDatabaseItemModel newItem);
    void itemEdited(WebsiteListDatabaseItemModel editedItem);

private Q_SLOTS:
    void addThisItemButtonPressed();

    void _useFileHippoCheckSectionButtonPressed();

private:
    void __init();
    void __actualizeGuiByItemForEdit();

private:
    QLineEdit *_appNameLineEdit;
    QLineEdit *_checkUrlLineEdit;
    QLineEdit *_downloadUrlLineEdit;
    QTextEdit *_noteTextEdit;
    QTextEdit *_checkSectionBeginTextExit;
    QTextEdit *_checkSectionEndTextExit;

    bool _isInEditMode;
    WebsiteListDatabaseItemModel _itemForEdit;
};

#endif // ADDNEWITEM_H
