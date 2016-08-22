#ifndef DELETECOLLECTIONCOMMAND_H
#define DELETECOLLECTIONCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QPair>
#include "sources/database/databaseservice.h"
#include "sources/database/collection.h"

class DeleteCollectionCommand : public QUndoCommand
{
public:

    //====================================================================================
    // Constructors
    //====================================================================================

    DeleteCollectionCommand(const QString& a_sCollectionName, QUndoCommand *parent = 0);


    //====================================================================================
    // Operations
    //====================================================================================

    void                            undo() Q_DECL_OVERRIDE;
    void                            redo() Q_DECL_OVERRIDE;


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    QString                         m_sCollectionName;
    QList<QPair<QString, QString> > m_Games;
};

#endif // DELETECOLLECTIONCOMMAND_H
