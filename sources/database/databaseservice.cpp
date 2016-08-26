#include "databaseservice.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>


DatabaseService* DatabaseService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

DatabaseService::DatabaseService()
{

}


//====================================================================================
// Accessors
//====================================================================================

DatabaseService* DatabaseService::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new DatabaseService();
    }
    return m_pInstance;
}

Platform* DatabaseService::getPlatform(const QString& a_sName) const
{
    return m_Database.getPlatform(a_sName);
}

Game* DatabaseService::getGame(const QString& a_sPlatformName, const QString& a_sGameName) const
{
    return m_Database.getGame(a_sPlatformName, a_sGameName);
}

Collection* DatabaseService::getCollection(const QString& a_sName) const
{
    return m_Database.getCollection(a_sName);
}

QString DatabaseService::getPlatformFile(const QString& a_sName) const
{
    return m_Database.getPlatformFile(a_sName);
}

QList<Platform*> DatabaseService::getPlatforms() const
{
    return m_Database.getPlatforms();
}

QList<QMap<QString, Game*> > DatabaseService::getGames()
{
    return m_Database.getGames();
}

uint DatabaseService::getGamesCount() const
{
    return m_Database.getGamesCount();
}

QList<Collection*> DatabaseService::getCollections()
{
    return m_Database.getCollections();
}

QList<QString> DatabaseService::getPlatformFiles() const
{
    return m_Database.getPlatformFiles();
}


//====================================================================================
// Operations
//====================================================================================

Game* DatabaseService::createGame(const QString a_sName, const QString a_sRomPath, Platform* a_pPlatform)
{
    return m_Database.createGame(a_sName, a_sRomPath, a_pPlatform);
}

void DatabaseService::deleteGame(Game* a_pGame)
{
    m_Database.deleteGame(a_pGame);
}

Collection* DatabaseService::createCollection(const QString a_sName)
{
    Collection* pCollection = m_Database.createCollection(a_sName);
    emit collectionCreated(pCollection);

    return pCollection;
}

void DatabaseService::deleteCollection(Collection* a_pCollection)
{
    emit collectionDeleted(a_pCollection);
    m_Database.deleteCollection(a_pCollection);
}

void DatabaseService::loadPlatforms()
{
    m_Database.loadPlatforms();
}

void DatabaseService::loadCollections()
{
    m_Database.loadCollections();
}

void DatabaseService::saveCollection(const QString& a_sName)
{
    m_Database.saveCollection(a_sName);
}