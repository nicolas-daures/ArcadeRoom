#ifndef CREATECOLLECTIONCOMMAND_H
#define CREATECOLLECTIONCOMMAND_H

#include <QUndoCommand>
#include "sources/database/databaseservice.h"
#include "sources/database/collection.h"

class CreateCollectionCommand : public QUndoCommand
{
public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a command to create a collection.
     * @param a_sCollectionName : name of the collection to create
     * @param a_pParent : parent command
     ***********************************************************/
    CreateCollectionCommand(const QString& a_sCollectionName, QUndoCommand* a_pParent = 0);


    //====================================================================================
    // Operations
    //====================================================================================

    void            undo() Q_DECL_OVERRIDE;
    void            redo() Q_DECL_OVERRIDE;


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    Collection*     m_pCollection;
    QString         m_sCollectionName;
};

#endif // CREATECOLLECTIONCOMMAND_H
