#ifndef COLLECTIONLISTWIDGET_H
#define COLLECTIONLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QToolButton>
#include <QUndoCommand>
#include "sources/database/collection.h"

namespace Ui
{
    class CollectionListWidget;
}

class CollectionListWidget : public QWidget
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a widget to show user collections.
     * @param a_pParent : parent widget
     ***********************************************************/
    explicit CollectionListWidget(QWidget* a_pParent = 0);
    ~CollectionListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    /***********************************************************
     * @brief Get the current list item.
     * @return current item if the list
     ***********************************************************/
    QListWidgetItem*            getCurrentItem();

    /***********************************************************
     * @brief Set the icons used by add and delete buttons.
     * @param a_addIcon : icon for add button
     * @param a_deleteIcon : icon for delete button
     ***********************************************************/
    void                        setIcons(const QIcon& a_addIcon, const QIcon& a_deleteIcon);


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Add the given collection to collection list.
     * @param a_sCollectionName : name of the collection to add
     ***********************************************************/
    void                        addItem(QString a_sCollectionName);


private slots:

    //====================================================================================
    // Private Slots
    //====================================================================================

    /***********************************************************
     * @brief Called when add collection button is clicked.
     *        Open a dialog box to create a new collection.
     ***********************************************************/
    void                        on_collectionAddButton_clicked();

    /***********************************************************
     * @brief Called when delete collection button is clicked.
     *        Delete the current selected collection.
     ***********************************************************/
    void                        on_collectionDeleteButton_clicked();

    /***********************************************************
     * @brief Called when a collection is clicked.
     *        Select the collection and display games.
     * @param a_pItem : selected item
     ***********************************************************/
    void                        on_collectionList_itemClicked(QListWidgetItem* a_pItem);

    /***********************************************************
     * @brief Called when the search collection button is pressed.
     *        Search a collection.
     ***********************************************************/
    void                        on_collectionSearch_returnPressed();

    /***********************************************************
     * @brief Called when a collection is created.
     *        Add the collection to collection list widget.
     ***********************************************************/
    void                        on_collectionCreated(Collection* a_pCollection);

    /***********************************************************
     * @brief Called when a collection is deleted.
     *        Remove the collection from collection list widget.
     ***********************************************************/
    void                        on_collectionDeleted(Collection* a_pCollection);


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::CollectionListWidget*   m_pUI;
};

#endif // COLLECTIONLISTWIDGET_H
