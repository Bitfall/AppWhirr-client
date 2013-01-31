#include "compareitemswindow.h"

#include <QWebView>
#include <QtWebKit/QWebFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>

CompareItemsWindow::CompareItemsWindow(CompareItemsWindowActionDelegate *actionDelegate, WebsiteListDatabaseItemModel originalItem, WebsiteListDatabaseItemModel newItem, QWidget *parent) :
    QMainWindow(parent),
    _actionDelegate(actionDelegate)
{
    this->_originalItem = originalItem;
    this->_newItem = newItem;

    this->resize(800, 500);

    QWidget *mainCentralWidget = new QWidget;
    this->setCentralWidget(mainCentralWidget);

    QGridLayout *mainLayout = new QGridLayout;
    mainCentralWidget->setLayout(mainLayout);


    //
    // title labels
    QLabel *originalTitleLabel = new QLabel;
    originalTitleLabel->setText(tr("Original content"));
    mainLayout->addWidget(originalTitleLabel, 0, 0, 1, 2);

    QLabel *newTitleLabel = new QLabel;
    newTitleLabel->setText(tr("New content"));
    mainLayout->addWidget(newTitleLabel, 0, 2, 1, 2);

    //
    // compare web-views
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

    _originalLinkContentPresenterWebView = new QWebView;
    _originalLinkContentPresenterWebView->setHtml(this->_originalItem.getContent());
    mainLayout->addWidget(_originalLinkContentPresenterWebView, 1, 0, 1, 2);
    connect(_originalLinkContentPresenterWebView->page(), SIGNAL(scrollRequested(int,int,QRect)), this, SLOT(originalLinkContentPresenterWebViewScroll(int,int,QRect)));

    _newLinkContentPresenterWebView = new QWebView;
    _newLinkContentPresenterWebView->setHtml(this->_newItem.getContent());
    connect(_newLinkContentPresenterWebView->page(), SIGNAL(scrollRequested(int,int,QRect)), this, SLOT(newLinkContentPresenterWebViewScroll(int,int,QRect)));
    mainLayout->addWidget(_newLinkContentPresenterWebView, 1, 2, 1, 2);

    // -- new row
    QLabel *checkSectionTitleLabel = new QLabel(tr("Check section begin/end:"));
    mainLayout->addWidget(checkSectionTitleLabel, 2, 0, 1, 1);

    _checkSectionBeginTextEdit = new QTextEdit;
    _checkSectionBeginTextEdit->setPlainText(this->_newItem.getCheckSectionBegin());
    mainLayout->addWidget(_checkSectionBeginTextEdit, 2, 1, 1, 2);

    _checkSectionEndTextEdit = new QTextEdit;
    _checkSectionEndTextEdit->setPlainText(this->_newItem.getCheckSectionEnd());
    mainLayout->addWidget(_checkSectionEndTextEdit, 2, 3, 1, 2);

    // -- new row
    QLabel *relatedNoteTitleLabel = new QLabel;
    relatedNoteTitleLabel->setText(tr("Related notes"));
    mainLayout->addWidget(relatedNoteTitleLabel, 3, 0);

    _noteTextEdit = new QTextEdit;
    _noteTextEdit->setPlainText(this->_newItem.getNote());
    mainLayout->addWidget(_noteTextEdit, 3, 1, 1, 3);

    //
    // action buttons
    QPushButton *keepOriginalContentActionButton = new QPushButton;
    keepOriginalContentActionButton->setMinimumHeight(30);
    keepOriginalContentActionButton->setText(tr("Keep the original content"));
    connect(keepOriginalContentActionButton, SIGNAL(clicked()), this, SLOT(keepTheOriginalContentButtonPressed()));
    mainLayout->addWidget(keepOriginalContentActionButton, 4, 0, 2, 1);

    QPushButton *newVersionActionButton = new QPushButton;
    newVersionActionButton->setMinimumHeight(50);
    newVersionActionButton->setText(tr("Use the new content - and Mark as Updated"));
    connect(newVersionActionButton, SIGNAL(clicked()), this, SLOT(useTheNewContentAndUpdateButtonPressed()));
    mainLayout->addWidget(newVersionActionButton, 4, 1, 2, 1);

    QPushButton *noNewVersionButUseTheNewContentActionButton = new QPushButton;
    noNewVersionButUseTheNewContentActionButton->setMinimumHeight(30);
    noNewVersionButUseTheNewContentActionButton->setText(tr("Use the new content (without marking it as updated)"));
    connect(noNewVersionButUseTheNewContentActionButton, SIGNAL(clicked()), this, SLOT(useTheNewContentButNotAnUpdateButtonPressed()));
    mainLayout->addWidget(noNewVersionButUseTheNewContentActionButton, 4, 2, 2, 1);
}

void CompareItemsWindow::originalLinkContentPresenterWebViewScroll(int dx, int dy, const QRect& scrollViewRect)
{
    _newLinkContentPresenterWebView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, this->_originalLinkContentPresenterWebView->page()->mainFrame()->scrollBarValue(Qt::Vertical));
}

void CompareItemsWindow::newLinkContentPresenterWebViewScroll(int dx, int dy, const QRect& scrollViewRect)
{
    _originalLinkContentPresenterWebView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, this->_newLinkContentPresenterWebView->page()->mainFrame()->scrollBarValue(Qt::Vertical));
}

void CompareItemsWindow::keepTheOriginalContentButtonPressed()
{
    if(this->_actionDelegate != NULL) {
        //
        // set updateable values
        this->_originalItem.setNote(this->_noteTextEdit->toPlainText());
        this->_originalItem.setCheckSectionBegin(this->_checkSectionBeginTextEdit->toPlainText());
        this->_originalItem.setCheckSectionEnd(this->_checkSectionEndTextEdit->toPlainText());
        // store
        this->_actionDelegate->keepTheOriginalContentActionSelected(this->_originalItem);
    }
    this->close();
}

void CompareItemsWindow::useTheNewContentButNotAnUpdateButtonPressed()
{
    if(this->_actionDelegate != NULL) {
        //
        // set updateable values
        this->_newItem.setNote(this->_noteTextEdit->toPlainText());
        this->_newItem.setCheckSectionBegin(this->_checkSectionBeginTextEdit->toPlainText());
        this->_newItem.setCheckSectionEnd(this->_checkSectionEndTextEdit->toPlainText());
        // store
        this->_actionDelegate->useTheNewContentButNotAnUpdateActionSelected(this->_newItem);
    }
    this->close();
}

void CompareItemsWindow::useTheNewContentAndUpdateButtonPressed()
{
    if(this->_actionDelegate != NULL) {
        //
        // set updateable values
        this->_newItem.setNote(this->_noteTextEdit->toPlainText());
        this->_newItem.setCheckSectionBegin(this->_checkSectionBeginTextEdit->toPlainText());
        this->_newItem.setCheckSectionEnd(this->_checkSectionEndTextEdit->toPlainText());
        // store
        this->_actionDelegate->useTheNewContentAndUpdateActionSelected(this->_newItem);
    }
    this->close();
}
