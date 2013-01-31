#ifndef APPLICATIONLISTWITHSECTIONSNETWORKENTITY_H
#define APPLICATIONLISTWITHSECTIONSNETWORKENTITY_H

#include <QObject>
#include "Common/Communication/basenetworkentity.h"

#include "applicationbaseinfosnetworkentitymodel.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class ApplicationListWithSectionsNetworkEntity : public BaseNetworkEntity
{
    Q_OBJECT
public:
//    // --------------------------
//    // --- section item model ---
//    class ApplicationListSectionItemModel
//    {
//    public:
//        explicit ApplicationListSectionItemModel();

//        void setApplicationID(QString applicationID);
//        void setApplicationName(QString applicationName);
//        void setApplicationIconURL(QString applicationIconURL);
//        void setApplicationVersion(QString applicationVersion);
//        void setApplicationCategory(QString applicationCategory);
//        void setApplicationVersionSummedFlags(const int& applicationSummedFlags);
//        void setApplicationRatingCount(const int& applicationRatingCount);
//        void setApplicationRating(const float& applicationRating);
//        void setApplicationReviewCount(const int& applicationReviewCount);

//        QString getApplicationID();
//        QString getApplicationName();
//        QString getApplicationVersion();
//        QString getApplicationIconURL();
//        QString getApplicationCategory();
//        int getApplicationVersionSummedFlags();
//        int getApplicationRatingCount();
//        float getApplicationRating();
//        int getApplicationReviewCount();

//    private:
//        QString             applicationID;
//        QString             applicationName;
//        QString             applicationIconURL;
//        QString             applicationVersion;
//        QString             applicationCategory;
//        int                 applicationVersionSummedFlags;
//        int                 applicationRatingCount;
//        float               applicationRating;
//        int                 applicationReviewCount;
//    };

    // ---------------------
    // --- section model ---
    class ApplicationListSectionModel
    {
    public:
        explicit ApplicationListSectionModel() {}
        explicit ApplicationListSectionModel(QString sectionTitle, QString sectionBackgroundImageUrl);

        void addItemToSection(ApplicationBaseInfosNetworkEntityModel itemModel);
        QList<ApplicationBaseInfosNetworkEntityModel> getItemsInSection() const;

        QString getSectionTitle() const;
        QString getSectionBackgroundImageUrl() const;
        bool isEmpty() const;

    private:
        QString _sectionTitle;
        QString _sectionBackgroundImageUrl;
        QList<ApplicationBaseInfosNetworkEntityModel> _itemsInSection;
    };

public:
    explicit ApplicationListWithSectionsNetworkEntity(QObject *parent = 0);

    void addSection(ApplicationListSectionModel sectionModel);
    QList<ApplicationListSectionModel> getSections() const;

    bool isEmpty() const;

private:
    QList<ApplicationListSectionModel> _sectionModels;
};

#endif // APPLICATIONLISTWITHSECTIONSNETWORKENTITY_H
