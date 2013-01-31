#ifndef CHECKBOXBUTTON_H
#define CHECKBOXBUTTON_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QCheckBox;
QT_END_NAMESPACE

class CheckboxButton : public QWidget
{
    Q_OBJECT
public:
    explicit CheckboxButton(QWidget *parent = 0);
    explicit CheckboxButton(QString titleText, QWidget *parent = 0);

    void setTitleText(QString titleText);
//    void setCheckboxChecked(bool isChecked);

    //
    // useful for CSS style-ing
//    static const QString getDefaultObjectName() { return QString("CheckboxButton"); }

    bool getIsChecked() const;

Q_SIGNALS:
    void toggled(bool isChecked);
    void clicked();
    void pressed();
    void released();

public Q_SLOTS:
    void setChecked(bool isChecked);

protected:
    void paintEvent(QPaintEvent *e);

    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    void __init(QString titleText = QString());

private:
//    QString _titleText;
    QLabel *_titleLabel;
    QCheckBox *_checkbox;
};

#endif // CHECKBOXBUTTON_H
