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

    /***********************************************************
     * @brief Create a widget to show platform overview
     *        (name, image, description, screenshots, ...).
     * @para a_pParent : parent widget
     ***********************************************************/
    explicit PlatformWidget(QWidget* a_pParent = 0);
    ~PlatformWidget();


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::PlatformWidget*     m_pUI;
};

#endif // PLATFORMWIDGET_H
