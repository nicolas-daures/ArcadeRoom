#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QUndoCommand>
#include "sources/database/database.h"

class CommandManager
{
public:

    //====================================================================================
    // Constructors
    //====================================================================================

    CommandManager(Database* a_pDatabase);


    //====================================================================================
    // Accessors
    //====================================================================================

    /***********************************************************
     * @brief Get all commands for given platform.
     * @param a_sPlatformName : platform's name
     * @return commands
     ***********************************************************/
    QMap<QString, QUndoCommand*>            getPlatformCommands(const QString& a_sPlatformName);

    /***********************************************************
     * @brief Get all commands for given collection.
     * @param a_sCollectionName : collection's name
     * @return commands
     ***********************************************************/
    QMap<QString, QUndoCommand*>            getCollectionCommands(const QString& a_sCollectionName);

    /***********************************************************
     * @brief Get all commands for given game.
     * @param a_sPlatformName : platform's name
     * @param a_sGameName : game's name
     * @return commands
     ***********************************************************/
    QMap<QString, QUndoCommand*>            getGameCommands(const QString& a_sPlatformName, const QString& a_sGameName);


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    Database*               m_pDatabase;
};

#endif // COMMANDMANAGER_H
