#include "commandservice.h"
#include "addgametocollectioncommand.h"
#include "removegamefromcollectioncommand.h"
#include <QString>


CommandService* CommandService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

CommandService::CommandService(Database* a_pDatabase, QMainWindow* a_pMainWindow)
: m_pDatabase(a_pDatabase)
{
    m_pUndoStack = new QUndoStack(a_pMainWindow);
#ifdef _DEBUG
    _createUndoView();
#endif
}


//====================================================================================
// Accessors
//====================================================================================

CommandService* CommandService::getInstance(Database* a_pDatabase, QMainWindow* a_pMainWindow)
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CommandService(a_pDatabase, a_pMainWindow);
    }
    return m_pInstance;
}

QMap<QString, QUndoCommand*> CommandService::getPlatformCommands(const QString&)
{
    QMap<QString, QUndoCommand*> commands;
    return commands;
}

QMap<QString, QUndoCommand*> CommandService::getCollectionCommands(const QString&)
{
    QMap<QString, QUndoCommand*> commands;
    return commands;
}

QMap<QString, QUndoCommand*> CommandService::getGameCommands(const QString& a_sPlatformName, const QString& a_sGameName)
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

#ifdef _DEBUG
void CommandService::_createUndoView()
{
    m_pUndoView = new QUndoView(m_pUndoStack);
    m_pUndoView->setWindowTitle("Command List");
    m_pUndoView->show();
    m_pUndoView->setAttribute(Qt::WA_QuitOnClose, false);
}
#endif


//====================================================================================
// Operations
//====================================================================================

void CommandService::undo()
{
    m_pUndoStack->undo();
}

void CommandService::redo()
{
    m_pUndoStack->redo();
}

void CommandService::push(QUndoCommand* a_pCommand)
{
    m_pUndoStack->push(a_pCommand);
}
