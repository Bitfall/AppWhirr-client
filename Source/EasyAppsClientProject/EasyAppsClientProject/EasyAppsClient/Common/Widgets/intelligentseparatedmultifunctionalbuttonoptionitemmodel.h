#ifndef IntelligentSeparatedMultifunctionalButtonWidgetOPTIONITEMMODEL_H
#define IntelligentSeparatedMultifunctionalButtonWidgetOPTIONITEMMODEL_H

#include <QObject>
#include <QString>
#include <QFont>

class IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel : public QObject
{
    Q_OBJECT
public:
    explicit IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel(QString titleText, QFont preferedFont, QObject *parent = 0);

    QString getTitleText() const;
    QFont getPreferedFont() const;

    void activateOption();

Q_SIGNALS:
    void optionActivated();

private:
    QString _titleText;
    QFont _preferedFont;
};

#endif // IntelligentSeparatedMultifunctionalButtonWidgetOPTIONITEMMODEL_H
