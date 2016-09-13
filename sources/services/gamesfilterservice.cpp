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

QList<Game*> GamesFilterService::getGames()
{
    return m_Games;
}

QList<Game*> GamesFilterService::getFilteredGames()
{
    return m_FilteredGames;
}

QString GamesFilterService::getSearchedString()
{
    return m_SearchedString;
}

void GamesFilterService::setGames(QList<Game*> a_Games)
{
    m_Games = a_Games;
}

void GamesFilterService::setFilteredGames(QList<Game*> a_Games)
{
    m_FilteredGames = a_Games;
}

void GamesFilterService::setSearchedString(const QString& a_String)
{
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
        // No filter, keep all games
        m_FilteredGames = a_Games;
    }
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

void GamesFilterService::loadGames(QString a_sPlatformName)
{
    Platform* pPlatform = DatabaseService::getInstance()->getPlatform(a_sPlatformName);
    if (pPlatform != NULL)
    {
        // Parse the roms directory
        QStringList filesAndDirectories;
        QDirIterator directories(pPlatform->getRomPath(),
                                 QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while(directories.hasNext())
        {
            directories.next();
            filesAndDirectories << directories.filePath();
        }

        // Get the platform
        QString sPlatformName = pPlatform->getName();

        // Fill game list
        m_Games = QList<Game*>();
        foreach (QString sGameDir, filesAndDirectories)
        {
            // Get rom files
            QStringList romFilter = createRomFilter(pPlatform);
            QDir directory(sGameDir);
            QStringList romFiles = directory.entryList(romFilter);

            if (romFiles.size() > 0)
            {
                // Add the game to database
                Game* pGame = DatabaseService::getInstance()->getGame(sPlatformName, romFiles[0]);
                if (pGame != NULL)
                {
                    m_Games.append(pGame);
                }
            }
        }
    }
}


//====================================================================================
// Queries
//====================================================================================

bool GamesFilterService::existsFilter() const
{
    return m_SearchedString != "";
}
