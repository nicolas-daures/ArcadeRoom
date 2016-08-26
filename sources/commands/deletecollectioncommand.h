#ifndef DELETECOLLECTIONCOMMAND_H
#define DELETECOLLECTIONCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QPair>
#include "sources/database/databaseservice.h"
#include "sources/database/collection.h"

class DeleteCollectionCommand : public QUndoCommand
{
public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a command to delete a collection.
     * @param a_sCollectionName : name of the collection to delete
     * @param a_pParent : parent command
     ***********************************************************/
    DeleteCollectionCommand(const QString& a_sCollectionName, QUndoCommand* a_pParent = 0);


    //====================================================================================
    // Operations
    //====================================================================================

    void                            undo() Q_DECL_OVERRIDE;
    void                            redo() Q_DECL_OVERRIDE;


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    QString                         m_sCollectionName;
    QList<QPair<QString, QString> > m_Games;
};

#endif // DELETECOLLECTIONCOMMAND_H
