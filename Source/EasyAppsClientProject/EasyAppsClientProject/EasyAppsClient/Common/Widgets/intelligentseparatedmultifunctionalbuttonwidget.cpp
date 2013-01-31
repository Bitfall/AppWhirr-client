#include "intelligentseparatedmultifunctionalbuttonwidget.h"

#include <QVBoxLayout>

#include "intelligentseparatedmultifunctionalbuttonoptionitemmodel.h"
#include "intelligentseparatedmultifunctionalbuttonoptionitemwidget.h"
#include "simpleimagepresenterwidget.h"

// utility
#include <QStylePainter>
//#include "Common/SvgToPixmapRenderer/svgtopixmaprenderer.h"
#include "Common/BorderImageRenderer/borderimagerenderer.h"
#include "Common/Helpers/textsizehelper.h"

#include <QDebug>

#define EXPAND_COLLAPSE_BUTTON_WIDTH 25
#define EXPAND_COLLAPSE_BUTTON_HEIGHT 25
#define ITEM_TEXT_TRESHOLD_WIDTH 20
#define ITEM_TEXT_TRESHOLD_HEIGHT 30
#define BUTTON_CONTENT_LEFT_MARGIN 10
#define BUTTON_CONTENT_RIGHT_MARGIN 10
#define BUTTON_CONTENT_TOP_MARGIN 3
#define BUTTON_CONTENT_BOTTOM_MARGIN 0

IntelligentSeparatedMultifunctionalButtonWidget::IntelligentSeparatedMultifunctionalButtonWidget(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *defaultOptionItemModel, QList<IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *> otherOptionItemModels, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, QPixmap expandCollapseButtonPixmap, QPixmap backgroundPixmap, QWidget *parent) :
    AnimatedTwoFixedSizeWidgetBase(1000, collapsedSize, expandedSize, isExpandedByDefault, true, true, parent)
{   
    mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    this->setContentsMargins(BUTTON_CONTENT_LEFT_MARGIN, BUTTON_CONTENT_TOP_MARGIN, BUTTON_CONTENT_RIGHT_MARGIN, BUTTON_CONTENT_BOTTOM_MARGIN);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
//    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize); // makes it flickering
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);

    // default option item line
    QWidget *defaultOptionLineWidget = new QWidget;
    this->defaultOptionItemLineHorizontalLayout = new QHBoxLayout;
    defaultOptionLineWidget->setLayout(this->defaultOptionItemLineHorizontalLayout);
    defaultOptionLineWidget->setContentsMargins(0, 0, 0, 0);
    this->defaultOptionItemLineHorizontalLayout->setContentsMargins(0, 0, 0, 0);
    this->defaultOptionItemLineHorizontalLayout->setSpacing(0);
    defaultOptionLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // default option item widget
    QSize defaultItemTextSize = TextSizeHelper::getTextSizeWithFont(defaultOptionItemModel->getTitleText(), defaultOptionItemModel->getPreferedFont());
    defaultItemTextSize = QSize(defaultItemTextSize.width() + ITEM_TEXT_TRESHOLD_WIDTH, defaultItemTextSize.height() + ITEM_TEXT_TRESHOLD_HEIGHT);
    this->defaultOptionItemWidget = this->createWidgetFromOptionItemModel(defaultOptionItemModel, defaultItemTextSize, defaultItemTextSize, true);
    this->defaultOptionItemLineHorizontalLayout->addWidget(this->defaultOptionItemWidget, 0, Qt::AlignLeft);

    this->expandCollapseImageWidget = new SimpleImagePresenterWidget();
    this->expandCollapseImageWidget->setImageByPixmap(expandCollapseButtonPixmap);
    connect(this->expandCollapseImageWidget, SIGNAL(clicked()), this, SLOT(expandCollapseWidgetClicked()));
    this->expandCollapseImageWidget->setFixedSize(EXPAND_COLLAPSE_BUTTON_WIDTH, EXPAND_COLLAPSE_BUTTON_HEIGHT);
    this->defaultOptionItemLineHorizontalLayout->addWidget(this->expandCollapseImageWidget, 0, Qt::AlignRight);

    this->mainLayout->addWidget(defaultOptionLineWidget);

    // other option items
    // add them to the layout, but hidden by default
    int otherOptionsCount = otherOptionItemModels.size();
    for(int i = 0; i < otherOptionsCount; i++) {
        IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *optionItemModel = otherOptionItemModels[i];
        QSize otherOptionItemTextSize = TextSizeHelper::getTextSizeWithFont(optionItemModel->getTitleText(), optionItemModel->getPreferedFont());
        otherOptionItemTextSize = QSize(otherOptionItemTextSize.width() + ITEM_TEXT_TRESHOLD_WIDTH, otherOptionItemTextSize.height() + ITEM_TEXT_TRESHOLD_HEIGHT);
        IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget *wrapperWidget = this->createWidgetFromOptionItemModel(optionItemModel, QSize(0, 0), otherOptionItemTextSize, this->getIsExpanded());
        this->mainLayout->addWidget(wrapperWidget);

        this->otherOptionItemWidgets.append(wrapperWidget);
    }


    // set the background
    this->backgroundPixmap = backgroundPixmap;
}

IntelligentSeparatedMultifunctionalButtonWidget *IntelligentSeparatedMultifunctionalButtonWidget::createWithItems(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *defaultOptionItemModel, QList<IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *> otherOptionItemModels, QPixmap expandCollapseButtonPixmap, QPixmap backgroundPixmap, QWidget *parent)
{
    int w = 0;
    int h = 0;

    // default item
    QFont defaultItemFont = defaultOptionItemModel->getPreferedFont();
    QSize defaultItemTextSize = TextSizeHelper::getTextSizeWithFont(defaultOptionItemModel->getTitleText(), defaultItemFont);
    w = qMax(defaultItemTextSize.width() + ITEM_TEXT_TRESHOLD_WIDTH + EXPAND_COLLAPSE_BUTTON_WIDTH + ITEM_TEXT_TRESHOLD_WIDTH, w);
    h += qMax(defaultItemTextSize.height() + ITEM_TEXT_TRESHOLD_HEIGHT, EXPAND_COLLAPSE_BUTTON_HEIGHT + ITEM_TEXT_TRESHOLD_HEIGHT);

    int otherOptionsCnt = otherOptionItemModels.size();
    for(int i = 0; i < otherOptionsCnt; i++) {
        IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *otherOptionItemModel = otherOptionItemModels[i];

        QFont font = otherOptionItemModel->getPreferedFont();
        QSize textSize = TextSizeHelper::getTextSizeWithFont(otherOptionItemModel->getTitleText(), font);
        w = qMax(textSize.width() + ITEM_TEXT_TRESHOLD_WIDTH, w);
        h += textSize.height() + ITEM_TEXT_TRESHOLD_HEIGHT;
    }

    QSize collapsedSize = QSize(defaultItemTextSize.width() + ITEM_TEXT_TRESHOLD_WIDTH + EXPAND_COLLAPSE_BUTTON_WIDTH + ITEM_TEXT_TRESHOLD_WIDTH, qMax(defaultItemTextSize.height() + ITEM_TEXT_TRESHOLD_HEIGHT, EXPAND_COLLAPSE_BUTTON_HEIGHT + ITEM_TEXT_TRESHOLD_HEIGHT));
    QSize expandedSize = QSize(w, h);

    return new IntelligentSeparatedMultifunctionalButtonWidget(defaultOptionItemModel, otherOptionItemModels, collapsedSize, expandedSize, false, expandCollapseButtonPixmap, backgroundPixmap, parent);
}

void IntelligentSeparatedMultifunctionalButtonWidget::paintEvent(QPaintEvent *e)
{

    QRect fullRect(0, 0, this->width() - 1, this->height() - 1);
    QPainter painter(this);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // finally, draw it
    BorderImageRenderer::renderPixmapAsBorderImageToPainter(this->backgroundPixmap, painter, fullRect, 10, 10, 15, 10);
}

IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget *IntelligentSeparatedMultifunctionalButtonWidget::createWidgetFromOptionItemModel(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *optionItemModel, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault)
{
    return new IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget(optionItemModel, collapsedSize, expandedSize, isExpandedByDefault);
}

void IntelligentSeparatedMultifunctionalButtonWidget::expandCollapseWidgetClicked()
{
//    qDebug() << "Current state: " << this->getIsExpanded();
    if(this->getIsExpanded()) {
        this->switchToCollapsedStateSlot();

        int otherOptionCnt = this->otherOptionItemWidgets.size();
        for(int i = 0; i < otherOptionCnt; i++) {
            this->otherOptionItemWidgets[i]->switchToCollapsedStateSlot();
        }
    }
    else {
        this->switchToExpandedStateSlot();

        int otherOptionCnt = this->otherOptionItemWidgets.size();
        for(int i = 0; i < otherOptionCnt; i++) {
//            this->otherOptionItemWidgets[i]->show();
            this->otherOptionItemWidgets[i]->switchToExpandedStateSlot();
        }
    }
}
