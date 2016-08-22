#ifndef ADDGAMETOCOLLECTIONCOMMAND_H
#define ADDGAMETOCOLLECTIONCOMMAND_H

#include <QUndoCommand>
#include "sources/database/collection.h"
#include "sources/database/game.h"
#include "sources/database/databaseservice.h"

class AddGameToCollectionCommand : public QUndoCommand
{
public:

    //====================================================================================
    // Constructors
    //====================================================================================

    AddGameToCollectionCommand(const QString& a_sCollectionName, const QString& a_sPlatformName, const QString& a_sGameName, QUndoCommand *parent = 0);


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

#endif // ADDGAMETOCOLLECTIONCOMMAND_H
