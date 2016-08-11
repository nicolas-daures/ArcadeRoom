#ifndef PLATFORMLISTWIDGET_H
#define PLATFORMLISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QComboBox>


namespace Ui
{
    class PlatformListWidget;
}

class PlatformListWidget : public QWidget
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    explicit PlatformListWidget(QWidget *parent = 0);
    ~PlatformListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    QTreeWidget*                getTreeWidget();
    QComboBox*                  getComboBoxPlatformSorting();
    QLineEdit*                  getTreeSearch();


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::PlatformListWidget*     m_pUI;
};

#endif // PLATFORMLISTWIDGET_H
