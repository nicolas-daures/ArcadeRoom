#ifndef PLATFORMWIDGET_H
#define PLATFORMWIDGET_H

#include <QWidget>

namespace Ui
{
    class PlatformWidget;
}

class PlatformWidget : public QWidget
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    explicit PlatformWidget(QWidget *parent = 0);
    ~PlatformWidget();


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::PlatformWidget*     m_pUI;
};

#endif // PLATFORMWIDGET_H
