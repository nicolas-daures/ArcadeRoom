#include "gamesfilterservice.h"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include "sources/database/databaseservice.h"


GamesFilterService* GamesFilterService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

GamesFilterService::GamesFilterService()
{
    m_Games = QList<Game*>();
    m_FilteredGames = QList<Game*>();
    m_DisplayedGames = QList<Game*>();
    m_SearchedString = "";
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

QList<Game*> GamesFilterService::getDisplayedGames()
{
    return m_DisplayedGames;
}

QList<Game*> GamesFilterService::getFilteredGames()
{
    return m_FilteredGames;
}

QString GamesFilterService::getSearchedString()
{
    return m_SearchedString;
}

void GamesFilterService::setDisplayedGames(QList<Game*> a_Games)
{
    m_DisplayedGames = a_Games;
}

void GamesFilterService::setFilteredGames(QList<Game*> a_Games)
{
    m_FilteredGames = a_Games;
}

void GamesFilterService::setGames(QList<Game*> a_Games)
{
    m_Games = a_Games;
}

void GamesFilterService::setSearchedString(const QString& a_String)
{
    m_SearchedString = a_String;
}


//====================================================================================
// Operations
//====================================================================================

void GamesFilterService::addDisplayedGame(Game* a_pGame)
{
    m_DisplayedGames.push_back(a_pGame);
}

// TODO obligé de faire un setgames avant applyfilter
// il est préférable peut etre de ne plus avoir de m_games
// et de lire directement la database dans apply filter.
void GamesFilterService::applyFilter()
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
        for (int iDirIndex = 0; iDirIndex < m_Games.size(); ++iDirIndex)
        {
            QString dir = m_Games[iDirIndex]->getName();
            if (dir.toLower().contains(searchText.toLower()))
            {
                m_FilteredGames.push_back(m_Games[iDirIndex]);
            }
        }
    }
    else
    {
        // No filter, keep all games
        m_FilteredGames = m_Games;
    }
}

void GamesFilterService::clearDisplayedGames()
{
    m_DisplayedGames.clear();
}

QStringList GamesFilterService::createRomFilter(Platform* a_pPlatform)
{
    QStringList romFilter;

    QStringList romExtensions = a_pPlatform->getRomExtensions();


    for (int iExtensionIndex = 0; iExtensionIndex < romExtensions.size(); ++iExtensionIndex)
    {
        QString extension = romExtensions[iExtensionIndex];
        romFilter << "*." + extension;
    }

    return romFilter;
}


//====================================================================================
// Queries
//====================================================================================

bool GamesFilterService::existsFilter() const
{
    return m_SearchedString != "";
}
