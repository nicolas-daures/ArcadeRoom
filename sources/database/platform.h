#ifndef PLATFORM_H
#define PLATFORM_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>

class Platform : public QObject
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    Platform(const QString a_sName = "",
             const QString a_sConstructorName = "",
             const QString a_sGeneration = "",
             const QString a_sEmulatorPath = "",
             const QStringList a_EmulatorArguments = QStringList(),
             const QString a_sRomPath = "",
             const QStringList m_RomExtensions = QStringList(),
             const QString a_sIconPath = "",
             const quint16 a_uiIconWidth = 150,
             const quint16 a_uiIconHeight = 150,
             const QString a_sDescription = "",
             const QString a_sImagePath = "",
             const QStringList a_ScreenshotUrls = QStringList(),
             const QStringList a_VideoUrls = QStringList());
    Platform(const Platform& a_Copy);


    //====================================================================================
    // Accessors
    //====================================================================================

    const QString&      getName() const;
    void                setName(const QString& a_sName);

    const QString&      getConstructorName() const;
    void                setConstructorName(const QString& a_sConstructorName);

    const QString&      getGeneration() const;
    void                setGeneration(const QString& a_sGeneration);

    const QString&      getEmulatorPath() const;
    void                setEmulatorPath(const QString& a_sEmulatorPath);

    const QStringList&  getEmulatorArguments() const;
    void                setEmulatorArguments(const QStringList& a_EmulatorArguments);

    const QString&      getRomPath() const;
    void                setRomPath(const QString& a_sRomPath);

    const QStringList&  getRomExtensions() const;
    void                setRomExtensions(const QStringList& a_RomExtensions);

    const QString&      getIconPath() const;
    void                setIconPath(const QString& a_sIconPath);

    const quint16&      getIconWidth() const;
    void                setIconWidth(const quint16& a_uiIconWidth);

    const quint16&      getIconHeight() const;
    void                setIconHeight(const quint16& a_uiIconHeight);

    const QString&      getDescription() const;
    void                setDescription(const QString& a_sDescription);

    const QString&      getImagePath() const;
    void                setImagePath(const QString& a_sImagePath);

    const QStringList&  getScreenshotUrls() const;
    void                setScreenshotUrls(const QStringList& a_ScreenshotUrls);

    const QStringList&  getVideoUrls() const;
    void                setVideoUrls(const QStringList& a_VideoUrls);


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Deserialize the given JSON object.
     * @param a_json : JSON object to deserialize
     ***********************************************************/
    void                read(const QJsonObject& a_json);

    /***********************************************************
     * @brief Serialize this in the given JSON object.
     * @param a_json : JSON object
     ***********************************************************/
    void                write(QJsonObject& a_json) const;


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when the platform's name changed.
     * @param a_sName : new platform's name
     ***********************************************************/
    void                nameChanged(QString a_sName);

    /***********************************************************
     * @brief Called when the constructor's name changed.
     * @param a_sConstructorName : new constructor's name
     ***********************************************************/
    void                constructorNameChanged(QString a_sConstructorName);

    /***********************************************************
     * @brief Called when the generation changed.
     * @param a_sGeneration : new generation
     ***********************************************************/
    void                generationChanged(QString a_sGeneration);

    /***********************************************************
     * @brief Called when the emulator path changed.
     * @param a_sEmulatorPath : new emulator path
     ***********************************************************/
    void                emulatorPathChanged(QString a_sEmulatorPath);

    /***********************************************************
     * @brief Called when the emulator arguments changed.
     * @param a_EmulatorArguments : new emulator arguments
     ***********************************************************/
    void                emulatorArgumentsChanged(QStringList a_EmulatorArguments);

    /***********************************************************
     * @brief Called when a rom path changed.
     * @param a_sRomPath : new rom path
     ***********************************************************/
    void                romPathChanged(QString a_sRomPath);

    /***********************************************************
     * @brief Called when the rom extensions changed.
     * @param a_RomExtensions : new rom extensions
     ***********************************************************/
    void                romExtensionsChanged(QStringList a_RomExtensions);

    /***********************************************************
     * @brief Called when the icon path changed.
     * @param a_sIconPath : new icon path
     ***********************************************************/
    void                iconPathChanged(QString a_sIconPath);

    /***********************************************************
     * @brief Called when the icon width changed.
     * @param a_iIconWidth : new icon width
     ***********************************************************/
    void                iconWidthChanged(quint16 a_iIconWidth);

    /***********************************************************
     * @brief Called when the icon height changed.
     * @param a_iIconHeight : new icon height
     ***********************************************************/
    void                iconHeightChanged(quint16 a_iIconHeight);

    /***********************************************************
     * @brief Called when the description changed.
     * @param a_sDescription : new description
     ***********************************************************/
    void                descriptionChanged(QString a_sDescription);

    /***********************************************************
     * @brief Called when the image path changed.
     * @param a_sImagePath : new image path
     ***********************************************************/
    void                imagePathChanged(QString a_sImagePath);

    /***********************************************************
     * @brief Called when the screenshot urls changed.
     * @param a_ScreenshotUrls : new screenshot urls
     ***********************************************************/
    void                screenshotUrlsChanged(QStringList a_ScreenshotUrls);

    /***********************************************************
     * @brief Called when the video urls changed.
     * @param a_VideoUrls : new video urls
     ***********************************************************/
    void                videoUrlsChanged(QStringList a_VideoUrls);


protected:

    //====================================================================================
    // Fields
    //====================================================================================

    QString             m_sName;
    QString             m_sConstructorName;
    QString             m_sGeneration;
    QString             m_sEmulatorPath;
    QStringList         m_EmulatorArguments;
    QString             m_sRomPath;
    QStringList         m_RomExtensions;
    QString             m_sIconPath;
    quint16             m_uiIconWidth;
    quint16             m_uiIconHeight;

    QString             m_sDescription;
    QString             m_sImagePath;
    QStringList         m_ScreenshotUrls;
    QStringList         m_VideoUrls;
};

#endif // PLATFORM_H
