#include "platform.h"
#include <iostream>
#include <QJsonArray>
#include <stdio.h>


//====================================================================================
// Constructors
//====================================================================================

Platform::Platform( const QString a_sName,
                    const QString a_sConstructorName,
                    const QString a_sGeneration,
                    const QString a_sEmulatorPath,
                    const QStringList a_EmulatorArguments,
                    const QString a_sRomPath,
                    const QStringList a_RomExtensions,
                    const QString a_sIconPath,
                    const quint16 a_uiIconWidth,
                    const quint16 a_uiIconHeight,
                    const QString a_sDescription,
                    const QString a_sImagePath,
                    const QStringList a_ScreenshotUrls,
                    const QStringList a_VideoUrls)
: m_sName(a_sName),
  m_sConstructorName(a_sConstructorName),
  m_sGeneration(a_sGeneration),
  m_sEmulatorPath(a_sEmulatorPath),
  m_EmulatorArguments(a_EmulatorArguments),
  m_sRomPath(a_sRomPath),
  m_RomExtensions(a_RomExtensions),
  m_sIconPath(a_sIconPath),
  m_uiIconWidth(a_uiIconWidth),
  m_uiIconHeight(a_uiIconHeight),
  m_sDescription(a_sDescription),
  m_sImagePath(a_sImagePath),
  m_ScreenshotUrls(a_ScreenshotUrls),
  m_VideoUrls(a_VideoUrls)
{

}

Platform::Platform(const Platform& a_Copy)
: QObject()
{
    m_sName = a_Copy.m_sName;
    m_sConstructorName = a_Copy.m_sConstructorName;
    m_sGeneration = a_Copy.m_sGeneration;
    m_sEmulatorPath = a_Copy.m_sEmulatorPath;
    m_EmulatorArguments = a_Copy.m_EmulatorArguments;
    m_sRomPath = a_Copy.m_sRomPath;
    m_RomExtensions = a_Copy.m_RomExtensions;
    m_sIconPath = a_Copy.m_sIconPath;
    m_uiIconWidth = a_Copy.m_uiIconWidth;
    m_uiIconHeight = a_Copy.m_uiIconHeight;
    m_sDescription = a_Copy.m_sDescription;
    m_sImagePath = a_Copy.m_sImagePath;
    m_ScreenshotUrls = a_Copy.m_ScreenshotUrls;
    m_VideoUrls = a_Copy.m_VideoUrls;
}


//====================================================================================
// Accessors
//====================================================================================

const QString& Platform::getName() const
{
    return m_sName;
}

void Platform::setName(const QString& a_sName)
{
    m_sName = a_sName;
    emit nameChanged(a_sName);
}

const QString& Platform::getConstructorName() const
{
    return m_sConstructorName;
}

void Platform::setConstructorName(const QString& a_sConstructorName)
{
    m_sConstructorName = a_sConstructorName;
    emit constructorNameChanged(a_sConstructorName);
}

const QString& Platform::getGeneration() const
{
    return m_sGeneration;
}

void Platform::setGeneration(const QString& a_sGeneration)
{
    m_sGeneration = a_sGeneration;
    emit generationChanged(a_sGeneration);
}

const QString& Platform::getEmulatorPath() const
{
    return m_sEmulatorPath;
}

void Platform::setEmulatorPath(const QString& a_sEmulatorPath)
{
    m_sEmulatorPath = a_sEmulatorPath;
    emit emulatorPathChanged(a_sEmulatorPath);
}

const QStringList& Platform::getEmulatorArguments() const
{
    return m_EmulatorArguments;
}

void Platform::setEmulatorArguments(const QStringList& a_EmulatorArguments)
{
    m_EmulatorArguments = a_EmulatorArguments;
    emit emulatorArgumentsChanged(a_EmulatorArguments);
}

const QString& Platform::getRomPath() const
{
    return m_sRomPath;
}

void Platform::setRomPath(const QString& a_sRomPath)
{
    m_sRomPath = a_sRomPath;
    emit romPathChanged(a_sRomPath);
}

const QStringList& Platform::getRomExtensions() const
{
    return m_RomExtensions;
}

void Platform::setRomExtensions(const QStringList& a_RomExtensions)
{
    m_RomExtensions = a_RomExtensions;
    emit romExtensionsChanged(a_RomExtensions);
}

const QString& Platform::getIconPath() const
{
    return m_sIconPath;
}

void Platform::setIconPath(const QString& a_sIconPath)
{
    m_sIconPath = a_sIconPath;
    emit iconPathChanged(a_sIconPath);
}

const quint16& Platform::getIconWidth() const
{
    return m_uiIconWidth;
}

void Platform::setIconWidth(const quint16& a_uiIconWidth)
{
    m_uiIconWidth = a_uiIconWidth;
    emit iconWidthChanged(a_uiIconWidth);
}

const quint16& Platform::getIconHeight() const
{
    return m_uiIconHeight;
}

void Platform::setIconHeight(const quint16& a_uiIconHeight)
{
    m_uiIconHeight = a_uiIconHeight;
    emit iconHeightChanged(a_uiIconHeight);
}

const QString& Platform::getDescription() const
{
    return m_sDescription;
}

void Platform::setDescription(const QString& a_sDescription)
{
    m_sDescription = a_sDescription;
    emit descriptionChanged(a_sDescription);
}

const QString& Platform::getImagePath() const
{
    return m_sImagePath;
}

void Platform::setImagePath(const QString& a_sImagePath)
{
    m_sImagePath = a_sImagePath;
    emit imagePathChanged(a_sImagePath);
}

const QStringList& Platform::getScreenshotUrls() const
{
    return m_ScreenshotUrls;
}

void Platform::setScreenshotUrls(const QStringList& a_ScreenshotUrls)
{
    m_ScreenshotUrls = a_ScreenshotUrls;
    emit screenshotUrlsChanged(a_ScreenshotUrls);
}

const QStringList& Platform::getVideoUrls() const
{
    return m_VideoUrls;
}

void Platform::setVideoUrls(const QStringList& a_VideoUrls)
{
    m_VideoUrls = a_VideoUrls;
    emit videoUrlsChanged(a_VideoUrls);
}


//====================================================================================
// Operations
//====================================================================================

void Platform::read(const QJsonObject& a_json)
{
    // Platform informations
    m_sName = a_json["name"].toString();
    m_sConstructorName = a_json["constructor_name"].toString();
    m_sGeneration = a_json["generation"].toString();

    // Emulator informations
    m_sEmulatorPath = a_json["emulator_path"].toString();
    m_EmulatorArguments.clear();
    QJsonArray emulatorArgumentArray = a_json["emulator_arguments"].toArray();
    QJsonArray::ConstIterator itArg = emulatorArgumentArray.constBegin();
    QJsonArray::ConstIterator endArg = emulatorArgumentArray.constEnd();
    for (; itArg != endArg; ++itArg)
    {
        QString argument = itArg->toString();
        m_EmulatorArguments.append(argument);
    }

    // Roms informations
    m_sRomPath = a_json["rom_path"].toString();
    m_RomExtensions.clear();
    QJsonArray romExtensionArray = a_json["rom_extensions"].toArray();
    QJsonArray::ConstIterator itExt = romExtensionArray.constBegin();
    QJsonArray::ConstIterator endExt = romExtensionArray.constEnd();
    for (; itExt != endExt; ++itExt)
    {
        QString extension = itExt->toString();
        m_RomExtensions.append(extension);
    }

    // Icon informations
    m_sIconPath = a_json["icon_path"].toString();
    m_uiIconWidth = a_json["icon_width"].toInt();
    m_uiIconHeight = a_json["icon_height"].toInt();

    // Description informations
    m_sDescription = a_json["description"].toString();
    m_sImagePath = a_json["image_path"].toString();
    m_ScreenshotUrls.clear();
    QJsonArray screenshotUrlsArray = a_json["screenshot_urls"].toArray();
    QJsonArray::ConstIterator itScreenshotUrl = screenshotUrlsArray.constBegin();
    QJsonArray::ConstIterator endScreenshotUrl = screenshotUrlsArray.constEnd();
    for (; itScreenshotUrl != endScreenshotUrl; ++itScreenshotUrl)
    {
        QString url = itScreenshotUrl->toString();
        m_ScreenshotUrls.append(url);
    }
    m_VideoUrls.clear();
    QJsonArray videoUrlsArray = a_json["video_urls"].toArray();
    QJsonArray::ConstIterator itUrl = videoUrlsArray.constBegin();
    QJsonArray::ConstIterator endUrl = videoUrlsArray.constEnd();
    for (; itUrl != endUrl; ++itUrl)
    {
        QString url = itUrl->toString();
        m_VideoUrls.append(url);
    }
}

void Platform::write(QJsonObject& a_json) const
{
    // Platform informations
    a_json["name"] = m_sName;
    a_json["constructor_name"] = m_sConstructorName;
    a_json["generation"] = m_sGeneration;

    // Emulator informations
    a_json["emulator_path"] = m_sEmulatorPath;
    QJsonArray emulatorArgumentArray;
    foreach (const QString argument, m_EmulatorArguments)
    {
        emulatorArgumentArray.append(argument);
    }
    a_json["emulator_arguments"] = emulatorArgumentArray;

    // Roms informations
    a_json["rom_path"] = m_sRomPath;
    QJsonArray romExtensionArray;
    foreach (const QString extension, m_RomExtensions)
    {
        romExtensionArray.append(extension);
    }
    a_json["rom_extensions"] = romExtensionArray;

    // Icon informations
    a_json["icon_path"] = m_sIconPath;
    a_json["icon_width"] = m_uiIconWidth;
    a_json["icon_height"] = m_uiIconHeight;

    // Description informations
    a_json["description"] = m_sDescription;
    a_json["image_path"] = m_sImagePath;
    QJsonArray screenshotUrlsArray;
    foreach (const QString url, m_ScreenshotUrls)
    {
        screenshotUrlsArray.append(url);
    }
    a_json["screenshot_urls"] = screenshotUrlsArray;
    QJsonArray videoUrlsArray;
    foreach (const QString url, m_VideoUrls)
    {
        videoUrlsArray.append(url);
    }
    a_json["video_urls"] = videoUrlsArray;
}
