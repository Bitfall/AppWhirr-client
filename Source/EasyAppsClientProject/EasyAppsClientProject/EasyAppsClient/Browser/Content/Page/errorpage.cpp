#include "errorpage.h"
#include "ui_errorpage.h"

ErrorPage::ErrorPage(QString pageId, QWidget *parent) :
    PageWidgetBase(pageId, parent),
    ui(new Ui::ErrorPage)
{
    ui->setupUi(this);
}

ErrorPage::~ErrorPage()
{
    delete ui;
}

void ErrorPage::initializePageContent()
{
}

void ErrorPage::pagePushWillHappen()
{
}

void ErrorPage::pageWillAppear(QString param)
{

}

void ErrorPage::pageDidAppear()
{
//    qDebug("page did appear");
}

void ErrorPage::pagePopWillHappen()
{

}

void ErrorPage::pageWillDisappear()
{

}
