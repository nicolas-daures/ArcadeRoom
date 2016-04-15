#include "preferences.h"


//====================================================================================
// Constructors
//====================================================================================

Preferences::Preferences()
: m_sLanguage("en"),
  m_sStyleName("Dark"),
  m_bShowNoCovers(true),
  m_bShowConsoles(true),
  m_bShowToolsBar(true),
  m_bShowStatusBar(true),
  m_fCoverSizeFactor(1.0f),
  m_iLayoutType(0)
{

}


//====================================================================================
// Accessors
//====================================================================================

const QString& Preferences::getLanguage() const
{
    return m_sLanguage;
}

void Preferences::setLanguage(const QString& a_sLanguage)
{
    m_sLanguage = a_sLanguage;
    emit languageChanged(a_sLanguage);
}

const QString& Preferences::getStyleName() const
{
    return m_sStyleName;
}

void Preferences::setStyleName(const QString& a_sStyleName)
{
    m_sStyleName = a_sStyleName;
    emit styleNameChanged(a_sStyleName);
}

bool Preferences::getShowNoCovers() const
{
    return m_bShowNoCovers;
}

void Preferences::setShowNoCovers(bool a_bShowNoCovers)
{
    m_bShowNoCovers = a_bShowNoCovers;
    emit showNoCoversChanged(a_bShowNoCovers);
}

bool Preferences::getShowConsoles() const
{
    return m_bShowConsoles;
}

void Preferences::setShowConsoles(bool a_bShowConsoles)
{
    m_bShowConsoles = a_bShowConsoles;
    emit showConsolesChanged(a_bShowConsoles);
}

bool Preferences::getShowToolsBar() const
{
    return m_bShowToolsBar;
}

void Preferences::setShowToolsBar(bool a_bShowToolsBar)
{
    m_bShowToolsBar = a_bShowToolsBar;
    emit showToolsBarChanged(a_bShowToolsBar);
}

bool Preferences::getShowStatusBar() const
{
    return m_bShowStatusBar;
}

void Preferences::setShowStatusBar(bool a_bShowStatusBar)
{
    m_bShowStatusBar = a_bShowStatusBar;
    emit showStatusBarChanged(a_bShowStatusBar);
}

float Preferences::getCoverSizeFactor() const
{
    return m_fCoverSizeFactor;
}

void Preferences::setCoverSizeFactor(float a_fCoverSizeFactor)
{
    m_fCoverSizeFactor = a_fCoverSizeFactor;
    emit coverSizeFactorChanged(a_fCoverSizeFactor);
}

uint Preferences::getLayoutType() const
{
    return m_iLayoutType;
}

void Preferences::setLayoutType(uint a_uiLayoutType)
{
    m_iLayoutType = a_uiLayoutType;
    emit layoutTypeChanged(a_uiLayoutType);
}


//====================================================================================
// Operations
//====================================================================================

void Preferences::read(const QJsonObject& a_json)
{
    // Preferences informations
    m_sLanguage = a_json["language"].toString();
    m_sStyleName = a_json["style"].toString();
    m_bShowNoCovers = a_json["showNoCovers"].toBool();
    m_bShowConsoles = a_json["showConsoles"].toBool();
    m_bShowToolsBar = a_json["showToolsBar"].toBool();
    m_bShowStatusBar = a_json["showStatusBar"].toBool();
    m_fCoverSizeFactor = (float)a_json["coverSizeFactor"].toDouble();
    m_iLayoutType = (uint)a_json["layoutType"].toInt();
}

void Preferences::write(QJsonObject& a_json) const
{
    // Preferences informations
    a_json["language"] = m_sLanguage;
    a_json["style"] = m_sStyleName;
    a_json["showNoCovers"] = m_bShowNoCovers;
    a_json["showConsoles"] = m_bShowConsoles;
    a_json["showToolsBar"] = m_bShowToolsBar;
    a_json["showStatusBar"] = m_bShowStatusBar;
    a_json["coverSizeFactor"] = m_fCoverSizeFactor;
    a_json["layoutType"] = m_iLayoutType;
}
