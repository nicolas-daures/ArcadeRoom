#ifndef REMOVEGAMEFROMCOLLECTIONCOMMAND_H
#define REMOVEGAMEFROMCOLLECTIONCOMMAND_H

#include <QUndoCommand>
#include "sources/database/collection.h"
#include "sources/database/game.h"
#include "sources/database/database.h"

class RemoveGameFromCollectionCommand : public QUndoCommand
{
public:

    //====================================================================================
    // Constructors
    //====================================================================================

    RemoveGameFromCollectionCommand(Database* a_pDatabase, const QString& a_sCollectionName, const QString& a_sPlatformName, const QString& a_sGameName, QUndoCommand *parent = 0);


    //====================================================================================
    // Operations
    //====================================================================================

    void            undo() Q_DECL_OVERRIDE;
    void            redo() Q_DECL_OVERRIDE;


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    Database*       m_pDatabase;
    QString         m_sCollectionName;
    QString         m_sPlatformName;
    QString         m_sGameName;
};

#endif // REMOVEGAMEFROMCOLLECTIONCOMMAND_H
