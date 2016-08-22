#ifndef PLATFORMLISTWIDGET_H
#define PLATFORMLISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QComboBox>
#include "sources/database/databaseservice.h"

#define CRITERIA_CONSTRUCTOR      QT_TRANSLATE_NOOP("CRITERIA_PLATFORM","Constructor")
#define CRITERIA_GENERATION       QT_TRANSLATE_NOOP("CRITERIA_PLATFORM","Generation")
#define CRITERIA_NAME             QT_TRANSLATE_NOOP("CRITERIA_PLATFORM","Name")
#define MAX_CHAR_PER_CRITERIA     12

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

    // TODO : remove it
    QTreeWidget*                getTreeWidget();
    QComboBox*                  getComboBoxPlatformSorting();
    QLineEdit*                  getTreeSearch();


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Fill the tree view with loaded platforms.
     ***********************************************************/
    void                        fillTreeView();
    void                        refreshComboBoxPlatformSorting();


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::PlatformListWidget*     m_pUI;
};

#endif // PLATFORMLISTWIDGET_H
