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
    QList<Game*>                        getDisplayedGames();
    QList<Game*>                        getFilteredGames();
    QList<Game*>                        getGames();
    QString                             getSearchedString();

    void                                setFilteredGames(QList<Game*> a_Games);
    void                                setDisplayedGames(QList<Game*> a_Games);
    void                                setGames(QList<Game*> a_Games);
    void                                setSearchedString(const QString& a_string);

    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Add a game to the displayed game list.
     * @param a_pGame : game to add
     ***********************************************************/
    void                                addDisplayedGame(Game* a_pGame);

    /***********************************************************
     * @brief Apply filter to keep only games with matching name.
     ***********************************************************/
    void                                applyFilter();

    /***********************************************************
     * @brief Clear the displayed game list.
     ***********************************************************/
    void                                clearDisplayedGames();

    /***********************************************************
     * @brief Create the rom extension filter.
     * @param a_pPlatform : platform
     * @return rom filter
     ***********************************************************/
    QStringList                         createRomFilter(Platform* a_pPlatform);


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

    // List of displayed games
    QList<Game*>                        m_DisplayedGames;

    // String to search in title of games
    QString                             m_SearchedString;
};

#endif // GAMESFILTERSERVICE_H
