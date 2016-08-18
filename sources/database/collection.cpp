#include "collection.h"
#include <QJsonArray>


//====================================================================================
// Constructors
//====================================================================================

Collection::Collection(const QString a_sName)
: m_sName(a_sName)
{

}

Collection::Collection(const Collection& a_Copy)
: QObject()
{
    m_sName = a_Copy.m_sName;
}


//====================================================================================
// Accessors
//====================================================================================

const QString& Collection::getName() const
{
    return m_sName;
}

void Collection::setName(const QString& a_sName)
{
    m_sName = a_sName;
    emit nameChanged(m_sName);
}

Game* Collection::getGame(const QString& a_sPlatformName, const QString& a_sGameName) const
{
    return m_Games[a_sPlatformName][a_sGameName];
}

QList<Game*> Collection::getGames()
{
    QList<Game*> games;
    QList<QMap<QString, Game*> > gamesMap = m_Games.values();
    QList<QMap<QString, Game*> >::iterator it = gamesMap.begin();
    QList<QMap<QString, Game*> >::iterator end = gamesMap.end();
    for (; it != end; ++it)
    {
        QList<Game*> gamesList = it->values();
        QList<Game*>::iterator itGame = gamesList.begin();
        QList<Game*>::iterator endGame = gamesList.end();
        for (; itGame != endGame; ++itGame)
        {
            games.append(*itGame);
        }
    }

    return games;
}


//====================================================================================
// Operations
//====================================================================================

void Collection::read(const QJsonObject& a_json)
{
    m_sName = a_json["name"].toString();
    m_Games.clear();
}

void Collection::write(QJsonObject& a_json) const
{
    a_json["name"] = m_sName;

    QJsonArray gameArray;
    QList<QMap<QString, Game*> > gamesMap = m_Games.values();
    QList<QMap<QString, Game*> >::iterator it = gamesMap.begin();
    QList<QMap<QString, Game*> >::iterator end = gamesMap.end();
    for (; it != end; ++it)
    {
        QList<Game*> gamesList = it->values();
        QList<Game*>::iterator itGame = gamesList.begin();
        QList<Game*>::iterator endGame = gamesList.end();
        for (; itGame != endGame; ++itGame)
        {
            Game* pGame = *itGame;

            QJsonObject json;
            json["platform_name"] = pGame->getPlatform()->getName();
            json["game_name"] = pGame->getName();
            gameArray.append(json);
        }
    }
    a_json["games"] = gameArray;
}

void Collection::addGame(Game* a_pGame)
{
    QString sPlatformName = a_pGame->getPlatform()->getName();
    QString sGameName = a_pGame->getName();
    if (m_Games.contains(sPlatformName) == false)
    {
        m_Games[sPlatformName] = QMap<QString, Game*>();
    }
    m_Games[sPlatformName][sGameName] = a_pGame;
    emit gameAdded(a_pGame);
}

void Collection::removeGame(Game* a_pGame)
{
    QString sPlatformName = a_pGame->getPlatform()->getName();
    QString sGameName = a_pGame->getName();
    if (m_Games.contains(sPlatformName) && m_Games[sPlatformName].contains(sGameName))
    {
        m_Games[sPlatformName].remove(sGameName);
        if (m_Games[sPlatformName].size() <= 0)
        {
            m_Games.remove(sPlatformName);
        }
    }
    emit gameRemoved(a_pGame);
}


//====================================================================================
// Queries
//====================================================================================

bool Collection::containsGame(Game* a_pGame) const
{
    QString sPlatformName = a_pGame->getPlatform()->getName();
    QString sGameName = a_pGame->getName();

    return m_Games.contains(sPlatformName) && m_Games[sPlatformName].contains(sGameName);
}
