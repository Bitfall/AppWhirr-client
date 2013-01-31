#ifndef MULTISECTIONDATA_H
#define MULTISECTIONDATA_H

#include <QObject>

class SameSizeLayoutBase;

class MultiSectionData : public QObject {
    Q_OBJECT
public:
    MultiSectionData(QWidget *sectionWidget, SameSizeLayoutBase *sectionLayout, QString sectionIdentifier = "");

    QWidget *getSectionWidget() const;
    SameSizeLayoutBase *getSectionLayout() const;

public Q_SLOTS:
    void sectionActivated();

Q_SIGNALS:
    void sectionIdentifierRequested(QString);

private:
    QWidget *_sectionWidget;
    SameSizeLayoutBase *_sectionLayout;
    QString _sectionIdentifier;
};

#endif // MULTISECTIONDATA_H
