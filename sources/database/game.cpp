#include "game.h"


//====================================================================================
// Constructors
//====================================================================================

Game::Game(const QString a_sName,
           const QString a_sRomRelativePath,
           Platform* a_pPlatform,
           quint16 a_uiRating)
: m_sName(a_sName),
  m_sRomRelativePath(a_sRomRelativePath),
  m_pPlatform(a_pPlatform),
  m_uiRating(a_uiRating)
{

}

Game::Game(const Game& a_Game)
: QObject()
{
    m_sName = a_Game.m_sName;
    m_sRomRelativePath = a_Game.m_sRomRelativePath;
    m_pPlatform = a_Game.m_pPlatform;
    m_uiRating = a_Game.m_uiRating;
}


//====================================================================================
// Accessors
//====================================================================================

const QString& Game::getName() const
{
    return m_sName;
}

void Game::setName(const QString& a_sName)
{
    m_sName = a_sName;
    emit nameChanged(a_sName);
}

const QString& Game::getRomRelativePath() const
{
    return m_sRomRelativePath;
}

void Game::setRomRelativePath(const QString& a_sRomPath)
{
    m_sRomRelativePath = a_sRomPath;
    emit romRelativePathChanged(a_sRomPath);
}

Platform* Game::getPlatform() const
{
    return m_pPlatform;
}

void Game::setPlatform(Platform* a_pPlatform)
{
    m_pPlatform = a_pPlatform;
    emit platformChanged(a_pPlatform);
}

quint16 Game::getRating() const
{
    return m_uiRating;
}

void Game::setRating(quint16 a_uiRating)
{
    m_uiRating = a_uiRating;
    emit ratingChanged(a_uiRating);
}
