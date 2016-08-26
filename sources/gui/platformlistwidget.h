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

    /***********************************************************
     * @brief Create a widget to show platform list.
     * @param a_pParent : parent widget
     ***********************************************************/
    explicit PlatformListWidget(QWidget* a_pParent = 0);
    ~PlatformListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    /***********************************************************
     * @brief Get the current tree widget item.
     * @return current tree widget item
     ***********************************************************/
    QTreeWidgetItem*            getCurrentItem();


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Fill the tree view with loaded platforms.
     ***********************************************************/
    void                        fillTreeView();

    /***********************************************************
     * @brief Refresh platform sorting.
     ***********************************************************/
    void                        refreshComboBoxPlatformSorting();


private slots:

    //====================================================================================
    // Private Slots
    //====================================================================================

    /***********************************************************
     * @brief Called when a tree's item is clicked.
     *        Change the current platform.
     * @param a_pCurrent : current tree item
     * @param a_pPrevious : previous tree column
     ***********************************************************/
    void                        on_treeWidget_currentItemChanged(QTreeWidgetItem* a_pCurrent, QTreeWidgetItem* a_pPrevious);

    /***********************************************************
     * @brief Called when the search button in treeview is pressed.
     *        Search a platform.
     ***********************************************************/
    void                        on_treeSearch_returnPressed();

    /***********************************************************
     * @brief Called when select a platform sorting criteria
     * @param a_iIndex : new current index
     ***********************************************************/
    void                        on_comboBoxPlatformSorting_activated(int a_iIndex);


private:

    //====================================================================================
    // Private Operations
    //====================================================================================

    /***********************************************************
     * @brief Set the selected item in treeview.
     * @param a_sPlatformName : selected platform name.
     ***********************************************************/
    void                        _setCurrentTreeViewItem(const QString& a_sPlatformName);


    //====================================================================================
    // Fields
    //====================================================================================

    Ui::PlatformListWidget*     m_pUI;
};

#endif // PLATFORMLISTWIDGET_H
