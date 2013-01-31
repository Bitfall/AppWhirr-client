#ifndef ICONIMAGEWIDGET_H
#define ICONIMAGEWIDGET_H

#include <QImage>
#include <QPicture>
#include <QSvgRenderer>
#include <QPixmap>

#include "Common/imagetypesenum.h"
#include "Common/Widgets/simpleimagepresenterwidget.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class IconImageWidget : public SimpleImagePresenterWidget
{
    Q_OBJECT
    Q_PROPERTY(float rotationValue READ getRotationValue WRITE setRotationValue)

public:
    explicit IconImageWidget(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum initialState, QWidget *parent = 0);

    void setRelatedAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newState);
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum getRelatedAppState() const;

    //
    // input: p -> [0;1] | 0 = no rotation | 1 = max rotation
    void setRotationValue(float p);
    float getRotationValue() const;

protected:
    void paintEvent(QPaintEvent *e);


private:
    bool _isRotationValueChanged;
    float _rotationValue;

    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum _relatedAppState;
};

#endif // ICONIMAGEWIDGET_H
