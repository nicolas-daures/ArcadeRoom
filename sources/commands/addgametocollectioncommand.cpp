#include "addgametocollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

AddGameToCollectionCommand::AddGameToCollectionCommand(Database* a_pDatabase, const QString& a_sCollectionName, const QString& a_sPlatformName, const QString& a_sGameName, QUndoCommand *parent)
: QUndoCommand(parent),
  m_pDatabase(a_pDatabase),
  m_sCollectionName(a_sCollectionName),
  m_sPlatformName(a_sPlatformName),
  m_sGameName(a_sGameName)
{
    setText("Add game (" + m_sGameName + ") to collection (" + m_sCollectionName + ")");
}


//====================================================================================
// Operations
//====================================================================================

void AddGameToCollectionCommand::undo()
{
    Collection* pCollection = m_pDatabase->getCollection(m_sCollectionName);
    Game* pGame = m_pDatabase->getGame(m_sPlatformName, m_sGameName);
    if (pCollection != NULL && pGame != NULL)
    {
        pCollection->removeGame(pGame);
    }
}

void AddGameToCollectionCommand::redo()
{
    Collection* pCollection = m_pDatabase->getCollection(m_sCollectionName);
    Game* pGame = m_pDatabase->getGame(m_sPlatformName, m_sGameName);
    if (pCollection != NULL && pGame != NULL)
    {
        pCollection->addGame(pGame);
    }
}
