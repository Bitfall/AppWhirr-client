#include "intelligentseparatedmultifunctionalbuttonoptionitemmodel.h"

IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel::IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel(QString titleText, QFont preferedFont, QObject *parent) :
    QObject(parent),
    _titleText(titleText),
    _preferedFont(preferedFont)
{
}

QString IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel::getTitleText() const
{
    return this->_titleText;
}

QFont IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel::getPreferedFont() const
{
    return this->_preferedFont;
}

void IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel::activateOption()
{
    Q_EMIT optionActivated();
}
