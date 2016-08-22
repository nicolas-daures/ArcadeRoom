#include "deletecollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

DeleteCollectionCommand::DeleteCollectionCommand(const QString& a_sCollectionName, QUndoCommand *parent)
: QUndoCommand(parent),
  m_sCollectionName(a_sCollectionName)
{
    setText("Delete collection (" + a_sCollectionName + ")");
}


//====================================================================================
// Operations
//====================================================================================

void DeleteCollectionCommand::undo()
{
    // Create the collection
    Collection* pCollection = DatabaseService::getInstance()->createCollection(m_sCollectionName);

    // Add games
    for (int iPairIndex = 0; iPairIndex < m_Games.size(); ++iPairIndex)
    {
        QPair<QString, QString> pair = m_Games[iPairIndex];
        Game* pGame = DatabaseService::getInstance()->getGame(pair.first, pair.second);
        pCollection->addGame(pGame);
    }
}

void DeleteCollectionCommand::redo()
{
    // Get the collection
    Collection* pCollection = DatabaseService::getInstance()->getCollection(m_sCollectionName);

    // Keep games
    m_Games.clear();
    foreach (Game* pGame, pCollection->getGames())
    {
        m_Games.append(QPair<QString, QString>(pGame->getPlatform()->getName(), pGame->getName()));
    }

    // Delete the collection
    DatabaseService::getInstance()->deleteCollection(pCollection);
}
