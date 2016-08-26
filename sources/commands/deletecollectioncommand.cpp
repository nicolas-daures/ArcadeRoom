#include "deletecollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

DeleteCollectionCommand::DeleteCollectionCommand(const QString& a_sCollectionName, QUndoCommand* a_pParent)
: QUndoCommand(a_pParent),
  m_sCollectionName(a_sCollectionName)
{
    setText("Delete collection (" + a_sCollectionName + ")");
}


//====================================================================================
// Operations
//====================================================================================

void DeleteCollectionCommand::undo()
{
    DatabaseService* pDatabaseService = DatabaseService::getInstance();

    // Create the collection
    Collection* pCollection = pDatabaseService->createCollection(m_sCollectionName);

    // Add games
    for (int iPairIndex = 0; iPairIndex < m_Games.size(); ++iPairIndex)
    {
        QPair<QString, QString> pair = m_Games[iPairIndex];
        Game* pGame = pDatabaseService->getGame(pair.first, pair.second);
        pCollection->addGame(pGame);
    }
}

void DeleteCollectionCommand::redo()
{
    DatabaseService* pDatabaseService = DatabaseService::getInstance();

    // Get the collection
    Collection* pCollection = pDatabaseService->getCollection(m_sCollectionName);

    // Keep games
    m_Games.clear();
    foreach (Game* pGame, pCollection->getGames())
    {
        m_Games.append(QPair<QString, QString>(pGame->getPlatform()->getName(), pGame->getName()));
    }

    // Delete the collection
    pDatabaseService->deleteCollection(pCollection);
}
