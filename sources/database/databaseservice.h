#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include "database.h"

class DatabaseService : public QObject
{
    Q_OBJECT


private:

    //====================================================================================
    // Constructors
    //====================================================================================

    DatabaseService();


public:

    //====================================================================================
    // Accessors
    //====================================================================================

    static DatabaseService*             getInstance();

    Platform*                           getPlatform(const QString& a_sName) const;
    Game*                               getGame(const QString& a_sPlatformName, const QString& a_sGameName) const;
    Collection*                         getCollection(const QString& a_sName) const;
    QString                             getPlatformFile(const QString& a_sName) const;

    QList<Platform*>                    getPlatforms() const;
    QList<QMap<QString, Game*> >        getGames();
    uint                                getGamesCount() const;
    QList<Collection*>                  getCollections();
    QList<QString>                      getPlatformFiles() const;


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Create a new game.
     * @param a_sName : game's name
     * @param a_sRomPath : path of the rom
     * @param a_pPlatform : platform
     * @return created game
     ***********************************************************/
    Game*                               createGame(const QString a_sName = "",
                                                   const QString a_sRomPath = "",
                                                   Platform* a_pPlatform = NULL);

    /***********************************************************
     * @brief Delete the given game.
     * @param a_pGame : game to delete
     ***********************************************************/
    void                                deleteGame(Game* a_pGame);

    /***********************************************************
     * @brief Create a new collection.
     * @param a_sName : collection's name
     * @return created collection
     ***********************************************************/
    Collection*                         createCollection(const QString a_sName = "");

    /***********************************************************
     * @brief Delete the given collection.
     * @param a_pCollection : collection to delete
     ***********************************************************/
    void                                deleteCollection(Collection* a_pCollection);

    /***********************************************************
     * @brief Load platforms.
     ***********************************************************/
    void                                loadPlatforms();

    /***********************************************************
     * @brief Load collections.
     ***********************************************************/
    void                                loadCollections();

    /***********************************************************
     * @brief Save the given collection.
     * @param a_sName : collection's name
     ***********************************************************/
    void                                saveCollection(const QString& a_sName);


signals:

    //====================================================================================
    // Signals
    //====================================================================================



private:

    //====================================================================================
    // Fields
    //====================================================================================

    static DatabaseService*             m_pInstance;

    Database                            m_Database;
};

#endif // DATABASESERVICE_H
