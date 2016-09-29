#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QMap>
#include <QString>
#include "platform.h"
#include "game.h"
#include "collection.h"

class Database : public QObject
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a database which contains platforms, games, collections, ...
     ***********************************************************/
    Database();


    //====================================================================================
    // Accessors
    //====================================================================================

    Platform*                           getPlatform(const QString& a_sName) const;
    Game*                               getGame(const QString& a_sPlatformName, const QString& a_sGameName) const;
    Collection*                         getCollection(const QString& a_sName) const;
    QString                             getPlatformFile(const QString& a_sName) const;

    QList<Platform*>                    getPlatforms() const;
    QList<QMap<QString, Game*> >        getGames();
    QList<Game*>                        getGames(const QString& a_sPlatformName);
    uint                                getGamesCount() const;
    QList<Collection*>                  getCollections();
    QList<QString>                      getPlatformFiles() const;


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Create a new platform.
     * @param a_sName : platform's name
     * @param a_sConstructorName : constructor's name
     * @param a_sEmulatorPath : path of the emulator
     * @param a_EmulatorArguments : arguments to run emulator
     * @param a_sRomPath : path of the roms
     * @param m_RomExtensions : extensions of the roms
     * @param a_sIconPath : path of the icon
     * @param m_uiIconWidth : icon width
     * @param m_uiIconHeight : icon height
     * @param a_sDescriptionFilePath : Platform description file path
     * @return created platform
     ***********************************************************/
    Platform*                           createPlatform(const QString a_sName = "",
                                                       const QString a_sConstructorName = "",
                                                       const QString a_sGeneration = "",
                                                       const QString a_sEmulatorPath = "",
                                                       const QStringList a_EmulatorArguments = QStringList(),
                                                       const QString a_sRomPath = "",
                                                       const QStringList m_RomExtensions = QStringList(),
                                                       const QString a_sIconPath = "",
                                                       const quint16 m_uiIconWidth = 150,
                                                       const quint16 m_uiIconHeight = 150,
                                                       const QString a_sDescriptionFilePath = "");

    /***********************************************************
     * @brief Delete the given platform.
     * @param a_pPlatform : platform to delete
     ***********************************************************/
    void                                deletePlatform(Platform* a_pPlatform);

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
     * @brief Parse the rom list from given platform directory.
     * @param a_pPlatform : platform
     ***********************************************************/
    void                                parseGamesFromDirectory(Platform* a_pPlatform);

    /***********************************************************
     * @brief Remove all games from given platform.
     * @param a_pPlatform : platform to clean
     ***********************************************************/
    void                                removeAllGamesFromPlatform(Platform* a_pPlatform);

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
     * @brief Load games.
     * @param a_sPlatformName : platform for which games to be
     *                          loaded
     ***********************************************************/
    void                                loadGames(QString a_sPlatformName);

    /***********************************************************
     * @brief Load collections.
     ***********************************************************/
    void                                loadCollections();

    /***********************************************************
     * @brief Save the given collection.
     * @param a_sName : collection's name
     ***********************************************************/
    void                                saveCollection(const QString& a_sName);


public slots:

    //====================================================================================
    // Slots
    //====================================================================================

    /***********************************************************
     * @brief Called when a game is added to collection.
     *        Save the collection.
     * @param a_pGame : added game
     ***********************************************************/
    void                                on_gameAddedToCollection(Game* a_pGame);

    /***********************************************************
     * @brief Called when a game is removed from collection.
     *        Save the collection.
     * @param a_pGame : removed game
     ***********************************************************/
    void                                on_gameRemovedFromCollection(Game* a_pGame);

    /***********************************************************
     * @brief Called when an emulator path is changed in
     *        preferences window.
     *        Save this emulator path in matching platform
     *        description file.
     * @param emulatorPath : emulator path
     * @param PlatformName : platform name on which emulator
     *                       path has changed
     ***********************************************************/
    void                               on_emulatorPathChanged(QString emulatorPath,
                                                              QString platformName);

    /***********************************************************
     * @brief Called when a roms path is changed in preferences
     *        window.
     *        Save this roms path in matching platform
     *        description file.
     * @param romsPath     : roms path
     * @param PlatformName : platform name on which roms
     *                       path has changed
     ***********************************************************/
    void                               on_romsPathChanged(QString romsPath,
                                                          QString platformName);


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    QMap<QString, Platform*>             m_PlatformMap;
    QMap<QString, QMap<QString, Game*> > m_GameMap;
    QMap<QString, Collection*>           m_CollectionMap;
    QMap<QString, QString>               m_PlatformFileMap;

    uint                                 m_uiGameCount;
};

#endif // DATABASE_H
