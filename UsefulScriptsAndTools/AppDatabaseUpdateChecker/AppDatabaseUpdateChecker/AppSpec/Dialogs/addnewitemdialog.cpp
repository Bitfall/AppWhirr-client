#include "addnewitemdialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

AddOrEditItemDialog::AddOrEditItemDialog(QWidget *parent) :
    QMainWindow(parent),
    _isInEditMode(false)
{
    this->__init();
}

AddOrEditItemDialog::AddOrEditItemDialog(WebsiteListDatabaseItemModel editThisItem, QWidget *parent) :
    QMainWindow(parent),
    _isInEditMode(true),
    _itemForEdit(editThisItem)
{
    this->__init();
}

void AddOrEditItemDialog::__init()
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->resize(800, 500);

    QWidget *mainCentralWidget = new QWidget;
    this->setCentralWidget(mainCentralWidget);

    QGridLayout *mainLayout = new QGridLayout;
    mainCentralWidget->setLayout(mainLayout);

    int gridLayoutRowIdx = 0; // utility counter for Grid-layouting

    // -- first row
    QLabel *appNameLabel = new QLabel(tr("App name:"));
    mainLayout->addWidget(appNameLabel, gridLayoutRowIdx, 0, 1, 1);
    _appNameLineEdit = new QLineEdit;
    mainLayout->addWidget(_appNameLineEdit, gridLayoutRowIdx, 1, 1, 1);

    // -- new row
    gridLayoutRowIdx++;

    QLabel *urlToCheckLabel = new QLabel(tr("Url to check:"));
    mainLayout->addWidget(urlToCheckLabel, gridLayoutRowIdx, 0, 1, 1);
    _checkUrlLineEdit = new QLineEdit;
    mainLayout->addWidget(_checkUrlLineEdit, gridLayoutRowIdx, 1, 1, 1);

    // -- new row
    gridLayoutRowIdx++;

    QLabel *urlToDownloadLabel = new QLabel(tr("Download url (optional):"));
    mainLayout->addWidget(urlToDownloadLabel, gridLayoutRowIdx, 0, 1, 1);
    _downloadUrlLineEdit = new QLineEdit;
    mainLayout->addWidget(_downloadUrlLineEdit, gridLayoutRowIdx, 1, 1, 1);

    // -- new row
    gridLayoutRowIdx++;

    QLabel *checkSectionBeginEndLabel = new QLabel(tr("Specify check-section begin-end text to check only this section (optional):"));
    mainLayout->addWidget(checkSectionBeginEndLabel, gridLayoutRowIdx, 0, 1, 1);
    gridLayoutRowIdx++;
    QPushButton *useFileHippoCheckSectionButton = new QPushButton(tr("Use FileHippo check sections."));
    useFileHippoCheckSectionButton->setMinimumHeight(30);
    connect(useFileHippoCheckSectionButton, SIGNAL(clicked()), this, SLOT(_useFileHippoCheckSectionButtonPressed()));
    mainLayout->addWidget(useFileHippoCheckSectionButton, gridLayoutRowIdx, 0, 1, 1);
    // text edits in 2 rows
    _checkSectionBeginTextExit = new QTextEdit;
    mainLayout->addWidget(_checkSectionBeginTextExit, gridLayoutRowIdx-1, 1, 2, 2);
    _checkSectionEndTextExit = new QTextEdit;
    mainLayout->addWidget(_checkSectionEndTextExit, gridLayoutRowIdx-1, 3, 2, 2);

    // -- new row
    gridLayoutRowIdx++;

    QLabel *notesLabel = new QLabel(tr("Notes (optional):"));
    mainLayout->addWidget(notesLabel, gridLayoutRowIdx, 0, 1, 1);
    _noteTextEdit = new QTextEdit;
    mainLayout->addWidget(_noteTextEdit, gridLayoutRowIdx, 1, 1, -1);

    // -- new row
    gridLayoutRowIdx++;

    //
    // action buttons
    QPushButton *addThisItemButton = new QPushButton;
    addThisItemButton->setText(tr("Save"));
    connect(addThisItemButton, SIGNAL(clicked()), this, SLOT(addThisItemButtonPressed()));
    mainLayout->addWidget(addThisItemButton, gridLayoutRowIdx, 0);

    QPushButton *cancelButton = new QPushButton;
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    mainLayout->addWidget(cancelButton, gridLayoutRowIdx, 1);

    if(this->_isInEditMode) {
        this->__actualizeGuiByItemForEdit();
    }
}

void AddOrEditItemDialog::__actualizeGuiByItemForEdit()
{
    _appNameLineEdit->setText(this->_itemForEdit.getAppName());
    _checkUrlLineEdit->setText(this->_itemForEdit.getUrl());
    _downloadUrlLineEdit->setText(this->_itemForEdit.getDownloadUrl());
    _checkSectionBeginTextExit->setPlainText(this->_itemForEdit.getCheckSectionBegin());
    _checkSectionEndTextExit->setPlainText(this->_itemForEdit.getCheckSectionEnd());
    _noteTextEdit->setPlainText(this->_itemForEdit.getNote());
}

void AddOrEditItemDialog::addThisItemButtonPressed()
{
    QString timestamp;
    QString timestampAsText;
    QString content;
    if(this->_isInEditMode) {
        // copy from the edited item
        timestamp = this->_itemForEdit.getTimestamp();
        timestampAsText = this->_itemForEdit.getTimestampAsText();
        content = this->_itemForEdit.getContent();
    }

    WebsiteListDatabaseItemModel item(this->_appNameLineEdit->text(), this->_checkUrlLineEdit->text(), this->_downloadUrlLineEdit->text(), content, timestamp, timestampAsText, this->_checkSectionBeginTextExit->toPlainText().simplified(), this->_checkSectionEndTextExit->toPlainText().simplified(), this->_noteTextEdit->toPlainText());

    if(this->_isInEditMode) {
        Q_EMIT itemEdited(item);
    }
    else {
        Q_EMIT newItemAdded(item);
    }

    this->close();
}


void AddOrEditItemDialog::_useFileHippoCheckSectionButtonPressed()
{
    this->_checkSectionBeginTextExit->setPlainText("<head><title>");
    this->_checkSectionEndTextExit->setPlainText("</title>");
}
