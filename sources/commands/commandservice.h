#ifndef COMMANDSERVICE_H
#define COMMANDSERVICE_H

#include <QUndoCommand>
#include "sources/database/databaseservice.h"
#include <QMainWindow>
#include <QUndoStack>
#ifdef _DEBUG
#include <QUndoView>
#endif


class CommandService
{
private:

    //====================================================================================
    // Constructors
    //====================================================================================

    CommandService(QMainWindow* a_pMainWindow);


public:

    //====================================================================================
    // Accessors
    //====================================================================================

    /***********************************************************
     * @brief Get unique instance of this class.
     * @return unique instance of this class
     ***********************************************************/
    static CommandService*                  getInstance(QMainWindow* a_pMainWindow = NULL);

    /***********************************************************
     * @brief Get all commands for given platform.
     * @param a_sPlatformName : platform's name
     * @return commands
     ***********************************************************/
    QMap<QString, QUndoCommand*>            getPlatformCommands(const QString& a_sPlatformName);

    /***********************************************************
     * @brief Get all commands for given collection.
     * @param a_sCollectionName : collection's name
     * @return commands
     ***********************************************************/
    QMap<QString, QUndoCommand*>            getCollectionCommands(const QString& a_sCollectionName);

    /***********************************************************
     * @brief Get all commands for given game.
     * @param a_sPlatformName : platform's name
     * @param a_sGameName : game's name
     * @return commands
     ***********************************************************/
    QMap<QString, QUndoCommand*>            getGameCommands(const QString& a_sPlatformName, const QString& a_sGameName);


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Undo the last command.
     ***********************************************************/
    void                                    undo();

    /***********************************************************
     * @brief Redo the last command undone.
     ***********************************************************/
    void                                    redo();

    /***********************************************************
     * @brief Push a new command on the stack.
     * @param a_pCommand : command to push
     ***********************************************************/
    void                                    push(QUndoCommand* a_pCommand);


protected:

    //====================================================================================
    // Private Operations
    //====================================================================================

#ifdef _DEBUG
    /***********************************************************
     * @brief Create a view to display undo commands.
     ***********************************************************/
    void                                    _createUndoView();
#endif


    //====================================================================================
    // Fields
    //====================================================================================

    // Unique instance of this class
    static CommandService*                  m_pInstance;

    // Undo stack
    QUndoStack*                             m_pUndoStack;

#if _DEBUG
    // Undo view, only used in debug mode
    QUndoView*                              m_pUndoView;
#endif
};

#endif // COMMANDSERVICE_H
