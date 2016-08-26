#include "database.h"
#include <QApplication>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "platform.h"
#include "game.h"
#include "collection.h"


//====================================================================================
// Constructors
//====================================================================================

Database::Database()
: m_uiGameCount(0)
{

}


//====================================================================================
// Accessors
//====================================================================================

Platform* Database::getPlatform(const QString& a_sName) const
{
    return m_PlatformMap[a_sName];
}

Game* Database::getGame(const QString& a_sPlatformName, const QString& a_sGameName) const
{
    if (m_GameMap.contains(a_sPlatformName) == false)
    {
        return NULL;
    }
    return m_GameMap[a_sPlatformName][a_sGameName];
}

Collection* Database::getCollection(const QString& a_sName) const
{
    return m_CollectionMap[a_sName];
}

QString Database::getPlatformFile(const QString& a_sName) const
{
    return m_PlatformFileMap[a_sName];
}

QList<Platform*> Database::getPlatforms() const
{
    return m_PlatformMap.values();
}

QList<QMap<QString, Game*> > Database::getGames()
{
    return m_GameMap.values();
}

uint Database::getGamesCount() const
{
    return m_uiGameCount;
}

QList<Collection*> Database::getCollections()
{
    return m_CollectionMap.values();
}

QList<QString> Database::getPlatformFiles() const
{
    return m_PlatformFileMap.values();
}


//====================================================================================
// Operations
//====================================================================================

Platform* Database::createPlatform(const QString a_sName,
                              const QString a_sConstructorName,
                              const QString a_sGeneration,
                              const QString a_sEmulatorPath,
                              const QStringList a_EmulatorArguments,
                              const QString a_sRomPath,
                              const QStringList m_RomExtensions,
                              const QString a_sIconPath,
                              const quint16 m_uiIconWidth,
                              const quint16 m_uiIconHeight,
                              const QString a_sDescriptionFilePath)
{
    Platform* pPlatform = new Platform(a_sName, a_sConstructorName, a_sGeneration,
                                       a_sEmulatorPath, a_EmulatorArguments,
                                       a_sRomPath, m_RomExtensions,
                                       a_sIconPath, m_uiIconWidth, m_uiIconHeight);
    m_PlatformMap[pPlatform->getName()] = pPlatform;
    m_PlatformFileMap[pPlatform->getName()] = a_sDescriptionFilePath;
    return pPlatform;
}

void Database::deletePlatform(Platform* a_pPlatform)
{
    m_PlatformMap.remove(a_pPlatform->getName());
    m_PlatformFileMap.remove(a_pPlatform->getName());
    delete a_pPlatform;
}

Game* Database::createGame(const QString a_sName,
                           const QString a_sRomPath,
                           Platform* a_pPlatform)
{
    Game* pGame = new Game(a_sName, a_sRomPath, a_pPlatform);
    m_GameMap[a_pPlatform->getName()][a_sName] = pGame;
    m_uiGameCount++;
    return pGame;
}

void Database::deleteGame(Game* a_pGame)
{
    m_GameMap[a_pGame->getPlatform()->getName()].remove(a_pGame->getName());
    delete a_pGame;
}

Collection* Database::createCollection(const QString a_sName)
{
    Collection* pCollection = new Collection(a_sName);
    m_CollectionMap[pCollection->getName()] = pCollection;

    connect(pCollection, SIGNAL(gameAdded(Game*)), this, SLOT(on_gameAddedToCollection(Game*)));
    connect(pCollection, SIGNAL(gameRemoved(Game*)), this, SLOT(on_gameRemovedFromCollection(Game*)));

    return pCollection;
}

void Database::deleteCollection(Collection* a_pCollection)
{
    QFile file(QString(QApplication::applicationDirPath() + "/collections/" + a_pCollection->getName() + ".json"));
    file.remove();

    m_CollectionMap.remove(a_pCollection->getName());

    delete a_pCollection;
}

void Database::loadPlatforms()
{
    // Get platform description files
    QStringList platformFilter;
    platformFilter << "*.json";
    QDir directory(QApplication::applicationDirPath() + "/platforms");
    QStringList platformFiles = directory.entryList(platformFilter);

    // Deserialize each platform
    foreach (QString platformFilePath, platformFiles)
    {
        // Get absolute file name
        QString platformAbsoluteFilePath = directory.absolutePath() + "/" + platformFilePath;

        // Open the file
        QFile loadFile(platformAbsoluteFilePath);
        if (!loadFile.open(QIODevice::ReadOnly))
        {
            qWarning(QString("Couldn't open platform file : " + directory.absolutePath() + "/" + platformFilePath).toStdString().c_str());
            continue;
        }

        // Read and deserialize
        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

        // Add the new platform
        Platform* pPlatform = new Platform();
        pPlatform->read(loadDoc.object());
        m_PlatformMap[pPlatform->getName()] = pPlatform;

        // Stock platform description file path
        m_PlatformFileMap[pPlatform->getName()] = platformAbsoluteFilePath;
    }
}

void Database::loadCollections()
{
    // Get collection description files
    QStringList collectionFilter;
    collectionFilter << "*.json";
    QDir directory(QApplication::applicationDirPath() + "/collections");
    QStringList collectionFiles = directory.entryList(collectionFilter);

    // Deserialize each collection
    foreach (QString collectionFilePath, collectionFiles)
    {
        // Open the file
        QFile loadFile(directory.absolutePath() + "/" + collectionFilePath);
        if (!loadFile.open(QIODevice::ReadOnly))
        {
            qWarning(QString("Couldn't open collection file : " + directory.absolutePath() + "/" + collectionFilePath).toStdString().c_str());
            continue;
        }

        // Read and deserialize
        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

        // Add the new collection
        Collection* pCollection = new Collection();
        pCollection->read(loadDoc.object());
        QJsonArray gameArray = loadDoc.object()["games"].toArray();
        QJsonArray::ConstIterator it = gameArray.constBegin();
        QJsonArray::ConstIterator end = gameArray.constEnd();
        for (; it != end; ++it)
        {
            QJsonObject json = it->toObject();
            QString sPlatformName = json["platform_name"].toString();
            QString sGameName = json["game_name"].toString();
            Game* pGame = getGame(sPlatformName, sGameName);
            if (pGame != NULL)
            {
                pCollection->addGame(pGame);
            }
        }
        m_CollectionMap[pCollection->getName()] = pCollection;

        connect(pCollection, SIGNAL(gameAdded(Game*)), this, SLOT(on_gameAddedToCollection(Game*)));
        connect(pCollection, SIGNAL(gameRemoved(Game*)), this, SLOT(on_gameRemovedFromCollection(Game*)));
    }
}

void Database::saveCollection(const QString& a_sName)
{
    Collection* pCollection = m_CollectionMap[a_sName];
    if (pCollection != NULL)
    {
        QString path = QString(QApplication::applicationDirPath() + "/collections");
        QDir directory = QDir(path);

        // Create directory if do not exist
        if (!directory.exists())
        {
            directory.mkpath(path);
        }

        // Create save collection file
        QFile saveFile(QString(path + QString("/" + a_sName + ".json")));
        if (!saveFile.open(QIODevice::WriteOnly))
        {
            qWarning("Couldn't open collection file.");
            return;
        }

        // Write save file
        QJsonObject collectionObject;
        pCollection->write(collectionObject);
        QJsonDocument saveDoc(collectionObject);
        saveFile.write(saveDoc.toJson());
    }
}


//====================================================================================
// Slots
//====================================================================================

void Database::on_gameAddedToCollection(Game*)
{
    // Save collection on disk
    Collection* pCollection = dynamic_cast<Collection*>(sender());
    if(pCollection != NULL)
    {
        saveCollection(pCollection->getName());
    }
}

void Database::on_gameRemovedFromCollection(Game*)
{
    // Save collection on disk
    Collection* pCollection = dynamic_cast<Collection*>(sender());
    if(pCollection != NULL)
    {
        saveCollection(pCollection->getName());
    }
}

void Database::on_emulatorPathChanged(QString emulatorPath, QString platformName)
{
    // Platform and file to modify
    Platform* pPlatformToModify;

    // Find matching platform
    pPlatformToModify = m_PlatformMap[platformName];

    // Nothing to do if platform do not exists
    if (pPlatformToModify == NULL)
    {
        qWarning("[Database::on_emulatorPathChanged]Platform '%s' do not exists. Emulator path has not been saved.",platformName.toStdString().data());
        return;
    }

    // Update emulator path in matching platform
    pPlatformToModify->setEmulatorPath(emulatorPath);

    // Write json file
    QString platformFileName = m_PlatformFileMap[platformName];
    QFile saveFile(platformFileName);

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("[Database::on_emulatorPathChanged]Couldn't open platform file '%s'. Emulator path has not been saved.",platformFileName.toStdString().data());
        return;
    }

    QJsonObject gameObject;
    pPlatformToModify->write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
}

void Database::on_romsPathChanged(QString romsPath, QString platformName)
{
    // Platform and file to modify
    Platform* pPlatformToModify;

    // Find matching platform
    pPlatformToModify = m_PlatformMap[platformName];

    // Nothing to do if platform do not exists
    if (pPlatformToModify == NULL)
    {
        qWarning("[Database::on_romsPathChanged]Platform '%s' do not exists. Rom path has not been saved.",platformName.toStdString().data());
        return;
    }

    // Update emulator path in matching platform
    pPlatformToModify->setRomPath(romsPath);

    // Write json file
    QString platformFileName = m_PlatformFileMap[platformName];
    QFile saveFile(platformFileName);

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("[Database::on_romsPathChanged]Couldn't open platform file '%s'. Rom path has not been saved.",platformFileName.toStdString().data());
        return;
    }

    QJsonObject gameObject;
    pPlatformToModify->write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
}
