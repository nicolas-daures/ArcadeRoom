#include "removegamefromcollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

RemoveGameFromCollectionCommand::RemoveGameFromCollectionCommand(const QString& a_sCollectionName, const QString& a_sPlatformName, const QString& a_sGameName, QUndoCommand* a_pParent)
: QUndoCommand(a_pParent),
  m_sCollectionName(a_sCollectionName),
  m_sPlatformName(a_sPlatformName),
  m_sGameName(a_sGameName)
{
    setText("Remove game (" + a_sGameName + ") from collection (" + a_sCollectionName + ")");
}


//====================================================================================
// Operations
//====================================================================================

void RemoveGameFromCollectionCommand::undo()
{
    DatabaseService* pDatabaseService = DatabaseService::getInstance();

    Collection* pCollection = pDatabaseService->getCollection(m_sCollectionName);
    Game* pGame = pDatabaseService->getGame(m_sPlatformName, m_sGameName);
    if (pCollection != NULL && pGame != NULL)
    {
        pCollection->addGame(pGame);
    }
}

void RemoveGameFromCollectionCommand::redo()
{
    DatabaseService* pDatabaseService = DatabaseService::getInstance();

    Collection* pCollection = pDatabaseService->getCollection(m_sCollectionName);
    Game* pGame = pDatabaseService->getGame(m_sPlatformName, m_sGameName);
    if (pCollection != NULL && pGame != NULL)
    {
        pCollection->removeGame(pGame);
    }
}
