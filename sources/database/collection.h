#ifndef COLLECTION_H
#define COLLECTION_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QMap>
#include "game.h"

class Collection : public QObject
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a collection which contains games.
     * @param a_sName : name of the collection
     ***********************************************************/
    Collection(const QString a_sName = "");

    /***********************************************************
     * @brief Create a collection by copy.
     * @param a_Collection : collection source
     ***********************************************************/
    Collection(const Collection& a_Collection);


    //====================================================================================
    // Accessors
    //====================================================================================

    const QString&                      getName() const;
    void                                setName(const QString& a_sName);

    Game*                               getGame(const QString& a_sPlatformName, const QString& a_sGameName) const;
    QList<Game*>                        getGames();


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Deserialize the given JSON object.
     * @param a_json : JSON object to deserialize
     ***********************************************************/
    void                                read(const QJsonObject& a_json);

    /***********************************************************
     * @brief Serialize this in the given JSON object.
     * @param a_json : JSON object
     ***********************************************************/
    void                                write(QJsonObject& a_json) const;

    /***********************************************************
     * @brief Add the given game to the collection.
     * @param a_pGame : game to add
     ***********************************************************/
    void                                addGame(Game* a_pGame);

    /***********************************************************
     * @brief Remove the given game from the collection.
     * @param a_pGame : game to remove
     ***********************************************************/
    void                                removeGame(Game* a_pGame);


    //====================================================================================
    // Queries
    //====================================================================================

    /***********************************************************
     * @brief Test if the collection contains given game.
     * @param a_pGame : game to test
     ***********************************************************/
    bool                                containsGame(Game* a_pGame) const;


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when the collection's name changed.
     * @param a_sName : new collection's name
     ***********************************************************/
    void                                nameChanged(QString a_sName);

    /***********************************************************
     * @brief Called when a game is added to collection.
     * @param a_pGame : game added
     ***********************************************************/
    void                                gameAdded(Game* a_pGame);

    /***********************************************************
     * @brief Called when a game is removed from collection.
     * @param a_pGame : game removed
     ***********************************************************/
    void                                gameRemoved(Game* a_pGame);


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    QString                              m_sName;
    QMap<QString, QMap<QString, Game*> > m_Games;
};

#endif // COLLECTION_H
