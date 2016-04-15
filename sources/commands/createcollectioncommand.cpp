#include "createcollectioncommand.h"


//====================================================================================
// Constructors
//====================================================================================

CreateCollectionCommand::CreateCollectionCommand(Database* a_pDatabase, const QString& a_sCollectionName, QUndoCommand *parent)
: QUndoCommand(parent),
  m_pDatabase(a_pDatabase),
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
    m_pDatabase->deleteCollection(m_pCollection);
}

void CreateCollectionCommand::redo()
{
    m_pCollection = m_pDatabase->createCollection(m_sCollectionName);
}
