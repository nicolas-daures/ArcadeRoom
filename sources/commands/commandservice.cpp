#include "commandservice.h"
#include "addgametocollectioncommand.h"
#include "removegamefromcollectioncommand.h"
#include <QString>


CommandService* CommandService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

CommandService::CommandService(QMainWindow* a_pMainWindow)
{
    m_pUndoStack = new QUndoStack(a_pMainWindow);
#ifdef _DEBUG
    _createUndoView();
#endif
}


//====================================================================================
// Accessors
//====================================================================================

CommandService* CommandService::getInstance(QMainWindow* a_pMainWindow)
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CommandService(a_pMainWindow);
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
    Game* pGame = DatabaseService::getInstance()->getGame(a_sPlatformName, a_sGameName);

    // Add commands
    QList<Collection*> collections = DatabaseService::getInstance()->getCollections();
    foreach (Collection* pCollection, collections)
    {
        if (pCollection->containsGame(pGame) == false)
        {
            commands["Add to " + pCollection->getName()] = new AddGameToCollectionCommand(pCollection->getName(), a_sPlatformName, a_sGameName);
        }
    }
    foreach (Collection* pCollection, collections)
    {
        if (pCollection->containsGame(pGame))
        {
            commands["Remove from " + pCollection->getName()] = new RemoveGameFromCollectionCommand(pCollection->getName(), a_sPlatformName, a_sGameName);
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
