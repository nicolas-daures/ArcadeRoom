#include "commandmanager.h"
#include "addgametocollectioncommand.h"
#include "removegamefromcollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

CommandManager::CommandManager(Database* a_pDatabase)
: m_pDatabase(a_pDatabase)
{

}


//====================================================================================
// Accessors
//====================================================================================

QMap<QString, QUndoCommand*> CommandManager::getPlatformCommands(const QString&)
{
    QMap<QString, QUndoCommand*> commands;
    return commands;
}

QMap<QString, QUndoCommand*> CommandManager::getCollectionCommands(const QString&)
{
    QMap<QString, QUndoCommand*> commands;
    return commands;
}

QMap<QString, QUndoCommand*> CommandManager::getGameCommands(const QString& a_sPlatformName, const QString& a_sGameName)
{
    QMap<QString, QUndoCommand*> commands;

    // Get the game
    Game* pGame = m_pDatabase->getGame(a_sPlatformName, a_sGameName);

    // Add commands
    QList<Collection*> collections = m_pDatabase->getCollections();
    foreach (Collection* pCollection, collections)
    {
        if (pCollection->containsGame(pGame) == false)
        {
            commands["Add to " + pCollection->getName()] = new AddGameToCollectionCommand(m_pDatabase, pCollection->getName(), a_sPlatformName, a_sGameName);
        }
    }
    foreach (Collection* pCollection, collections)
    {
        if (pCollection->containsGame(pGame))
        {
            commands["Remove from " + pCollection->getName()] = new RemoveGameFromCollectionCommand(m_pDatabase, pCollection->getName(), a_sPlatformName, a_sGameName);
        }
    }
    //commands.push_front(new RemoveGameCommand(pGame));

    return commands;
}
