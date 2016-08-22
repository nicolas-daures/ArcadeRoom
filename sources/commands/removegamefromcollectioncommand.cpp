#include "removegamefromcollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

RemoveGameFromCollectionCommand::RemoveGameFromCollectionCommand(const QString& a_sCollectionName, const QString& a_sPlatformName, const QString& a_sGameName, QUndoCommand *parent)
: QUndoCommand(parent),
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
    Collection* pCollection = DatabaseService::getInstance()->getCollection(m_sCollectionName);
    Game* pGame = DatabaseService::getInstance()->getGame(m_sPlatformName, m_sGameName);
    if (pCollection != NULL && pGame != NULL)
    {
        pCollection->addGame(pGame);
    }
}

void RemoveGameFromCollectionCommand::redo()
{
    Collection* pCollection = DatabaseService::getInstance()->getCollection(m_sCollectionName);
    Game* pGame = DatabaseService::getInstance()->getGame(m_sPlatformName, m_sGameName);
    if (pCollection != NULL && pGame != NULL)
    {
        pCollection->removeGame(pGame);
    }
}
