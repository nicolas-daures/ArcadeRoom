#include "gamesfilterservice.h"
#include <QApplication>


GamesFilterService* GamesFilterService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

GamesFilterService::GamesFilterService()
{
    m_SearchedString = "";
    // TODO init Qlist ?
}


//====================================================================================
// Accessors
//====================================================================================

GamesFilterService* GamesFilterService::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new GamesFilterService();
    }
    return m_pInstance;
}

QList<Game*> GamesFilterService::getFilteredGames()
{
    return m_FilteredGames;
}

QString GamesFilterService::getSearchedString()
{
    return m_SearchedString;
}

void GamesFilterService::setFilteredGames(QList<Game*> a_Games)
{
    m_FilteredGames = a_Games;
}

void GamesFilterService::setSearchedString(const QString& a_String)
{
    // TODO tolower here ?
    m_SearchedString = a_String;
}


//====================================================================================
// Operations
//====================================================================================

void GamesFilterService::applyFilter(QList<Game*> a_Games)
{
    // Apply filter only if a filter has been set
    if (existsFilter())
    {
        // Clear existing filtered games
        m_FilteredGames.clear();

        // Get the filter (string to search to lower case)
        QString searchText = m_SearchedString.toLower();

        // Process each game of the list to keep only those with a name which contains
        // matching string
        for (int iDirIndex = 0; iDirIndex < a_Games.size(); ++iDirIndex)
        {
            QString dir = a_Games[iDirIndex]->getName();
            if (dir.toLower().contains(searchText.toLower()))
            {
                m_FilteredGames.push_back(a_Games[iDirIndex]);
            }
        }
    }
    else
    {
        // TODO : utile ? la boucle d'en haut va faire le même boulot sur une ""
        m_FilteredGames = a_Games;
    }
}


//====================================================================================
// Queries
//====================================================================================
// TODO utile ? laisser public ?
bool GamesFilterService::existsFilter() const
{
    return m_SearchedString != "";
}
