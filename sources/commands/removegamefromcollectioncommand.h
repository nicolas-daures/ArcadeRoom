#ifndef REMOVEGAMEFROMCOLLECTIONCOMMAND_H
#define REMOVEGAMEFROMCOLLECTIONCOMMAND_H

#include <QUndoCommand>
#include "sources/database/collection.h"
#include "sources/database/game.h"
#include "sources/database/databaseservice.h"

class RemoveGameFromCollectionCommand : public QUndoCommand
{
public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a command to remove a game from collection.
     * @param a_sCollectionName : name of the collection
     * @param a_sPlatformName : name of the platform
     * @param a_sGameName : name of the game to remove
     * @param a_pParent : parent command
     ***********************************************************/
    RemoveGameFromCollectionCommand(const QString& a_sCollectionName, const QString& a_sPlatformName, const QString& a_sGameName, QUndoCommand* a_pParent = 0);


    //====================================================================================
    // Operations
    //====================================================================================

    void            undo() Q_DECL_OVERRIDE;
    void            redo() Q_DECL_OVERRIDE;


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    QString         m_sCollectionName;
    QString         m_sPlatformName;
    QString         m_sGameName;
};

#endif // REMOVEGAMEFROMCOLLECTIONCOMMAND_H
