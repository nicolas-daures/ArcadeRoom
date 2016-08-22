#include "createcollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

CreateCollectionCommand::CreateCollectionCommand(const QString& a_sCollectionName, QUndoCommand *parent)
: QUndoCommand(parent),
  m_pCollection(NULL),
  m_sCollectionName(a_sCollectionName)
{
    setText("Create collection (" + a_sCollectionName + ")");
}


//====================================================================================
// Operations
//====================================================================================

void CreateCollectionCommand::undo()
{
    DatabaseService::getInstance()->deleteCollection(m_pCollection);
}

void CreateCollectionCommand::redo()
{
    m_pCollection = DatabaseService::getInstance()->createCollection(m_sCollectionName);
}
