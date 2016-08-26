#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include "platform.h"

class Game : public QObject
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a game.
     * @param a_sName : name of the game
     * @param a_sRomRelativePath : relative path of the rom file
     * @param a_pPlatform : platform (console)
     * @param a_uiRating : rating (stars)
     ***********************************************************/
    Game(const QString a_sName = "",
         const QString a_sRomRelativePath = "",
         Platform* a_pPlatform = NULL,
         quint16 a_uiRating = 3);

    /***********************************************************
     * @brief Create a game by copy.
     * @param a_Game : game source
     ***********************************************************/
    Game(const Game& a_Game);


    //====================================================================================
    // Accessors
    //====================================================================================

    const QString&      getName() const;
    void                setName(const QString& a_sName);

    const QString&      getRomRelativePath() const;
    void                setRomRelativePath(const QString& a_sRomPath);

    Platform*           getPlatform() const;
    void                setPlatform(Platform* a_pPlatform);

    quint16             getRating() const;
    void                setRating(quint16 a_uiRating);


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when the game's name changed.
     * @param a_sName : new game's name
     ***********************************************************/
    void                nameChanged(QString a_sName);

    /***********************************************************
     * @brief Called when a rom path changed.
     * @param a_sRomPath : new rom path
     ***********************************************************/
    void                romRelativePathChanged(QString a_sRomPath);

    /***********************************************************
     * @brief Called when the platform changed.
     * @param a_pPlatform : new platform
     ***********************************************************/
    void                platformChanged(Platform* a_pPlatform);

    /***********************************************************
     * @brief Called when the rating changed.
     * @param a_uiRating : new rating
     ***********************************************************/
    void                ratingChanged(quint16 a_uiRating);


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    QString             m_sName;
    QString             m_sRomRelativePath;
    Platform*           m_pPlatform;
    quint16             m_uiRating;
};

#endif // GAME_H
