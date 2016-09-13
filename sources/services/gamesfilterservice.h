#ifndef GAMESFILTERSERVICE_H
#define GAMESFILTERSERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "sources/database/game.h"

class GamesFilterService : public QObject
{
    Q_OBJECT


private:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a service to manage games filter.
     ***********************************************************/
    GamesFilterService();


public:

    //====================================================================================
    // Accessors
    //====================================================================================

    static GamesFilterService*          getInstance();
    QList<Game*>                        getGames();
    QList<Game*>                        getFilteredGames();
    QString                             getSearchedString();

    void                                setGames(QList<Game*> a_Games);
    void                                setFilteredGames(QList<Game*> a_Games);
    void                                setSearchedString(const QString& a_string);

    //====================================================================================
    // Accessors
    //====================================================================================

    /***********************************************************
     * @brief Apply filter to keep only games with matching name.
     * @param a_Games : list of games to filter
     ***********************************************************/
    void                                applyFilter(QList<Game*> a_Games);

    /***********************************************************
     * @brief Create the rom extension filter.
     * @param a_pPlatform : platform
     * @return rom filter
     ***********************************************************/
    QStringList                         createRomFilter(Platform* a_pPlatform);

    /***********************************************************
     * @brief Load games from a platform name.
     * @param a_sPlatformName : name of the platform.
     ***********************************************************/
    void                                loadGames(QString a_sPlatformName);


    //====================================================================================
    // Queries
    //====================================================================================

    /***********************************************************
     * @brief Test if a filter is active.
     ***********************************************************/
    bool                                existsFilter() const;


signals:

    //====================================================================================
    // Signals
    //====================================================================================


private:

    //====================================================================================
    // Fields
    //====================================================================================

    static GamesFilterService*          m_pInstance;

    // List of games
    QList<Game*>                        m_Games;

    // List of filtered games
    QList<Game*>                        m_FilteredGames;

    // String to search in title of games
    QString                             m_SearchedString;
};

#endif // GAMESFILTERSERVICE_H
