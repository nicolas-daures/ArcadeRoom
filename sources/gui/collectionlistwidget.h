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

    explicit CollectionListWidget(QWidget *parent = 0);
    ~CollectionListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    // TODO : remove it
    QListWidget*                getCollectionList();
    QToolButton*                getCollectionAddButton();
    QToolButton*                getCollectionDeleteButton();

    QString                     getCurrentCollection() const;
    void                        setCurrentCollection(QString a_sCollection);


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when a collection is selected.
     * @param a_pCollection : selected collection
     ***********************************************************/
    void                        collectionSelected(Collection* a_pCollection);

    /***********************************************************
     * @brief Called when a command is created.
     * @param a_pCommand : command created
     ***********************************************************/
    void                        commandCreated(QUndoCommand* a_pCommand);


private slots:

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


private:

    //====================================================================================
    // Private Operations
    //====================================================================================

    /***********************************************************
     * @brief Create a new collection.
     * @param a_sName : collection name
     ***********************************************************/
    void                        _createCollection(QString a_sName);

    /***********************************************************
     * @brief Delete the given collection.
     * @param a_sName : collection name
     ***********************************************************/
    void                        _deleteCollection(QString a_sName);


    //====================================================================================
    // Fields
    //====================================================================================

    Ui::CollectionListWidget*   m_pUI;
};

#endif // COLLECTIONLISTWIDGET_H
