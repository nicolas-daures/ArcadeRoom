#include "preferenceservice.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>


PreferenceService* PreferenceService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

PreferenceService::PreferenceService()
{

}


//====================================================================================
// Accessors
//====================================================================================

PreferenceService* PreferenceService::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new PreferenceService();
    }
    return m_pInstance;
}

const QString& PreferenceService::getLanguage() const
{
    return m_Preferences.getLanguage();
}

void PreferenceService::setLanguage(const QString& a_sLanguage)
{
    m_Preferences.setLanguage(a_sLanguage);
    save();
    emit languageChanged(a_sLanguage);
}

const QString& PreferenceService::getStyleName() const
{
    return m_Preferences.getStyleName();
}

void PreferenceService::setStyleName(const QString& a_sStyleName)
{
    m_Preferences.setStyleName(a_sStyleName);
    save();
    emit styleNameChanged(a_sStyleName);
}

bool PreferenceService::getShowNoCovers() const
{
    return m_Preferences.getShowNoCovers();
}

void PreferenceService::setShowNoCovers(bool a_bShowNoCovers)
{
    m_Preferences.setShowNoCovers(a_bShowNoCovers);
    save();
    emit showNoCoversChanged(a_bShowNoCovers);
}

bool PreferenceService::getShowConsoles() const
{
    return m_Preferences.getShowConsoles();
}

void PreferenceService::setShowConsoles(bool a_bShowConsoles)
{
    m_Preferences.setShowConsoles(a_bShowConsoles);
    save();
    emit showConsolesChanged(a_bShowConsoles);
}

bool PreferenceService::getShowToolsBar() const
{
    return m_Preferences.getShowToolsBar();
}

void PreferenceService::setShowToolsBar(bool a_bShowToolsBar)
{
    m_Preferences.setShowToolsBar(a_bShowToolsBar);
    save();
    emit showToolsBarChanged(a_bShowToolsBar);
}

bool PreferenceService::getShowStatusBar() const
{
    return m_Preferences.getShowStatusBar();
}

void PreferenceService::setShowStatusBar(bool a_bShowStatusBar)
{
    m_Preferences.setShowStatusBar(a_bShowStatusBar);
    save();
    emit showStatusBarChanged(a_bShowStatusBar);
}

float PreferenceService::getCoverSizeFactor() const
{
    return m_Preferences.getCoverSizeFactor();
}

void PreferenceService::setCoverSizeFactor(float a_fCoverSizeFactor)
{
    m_Preferences.setCoverSizeFactor(a_fCoverSizeFactor);
    save();
    emit coverSizeFactorChanged(a_fCoverSizeFactor);
}

uint PreferenceService::getLayoutType() const
{
    return m_Preferences.getLayoutType();
}

void PreferenceService::setLayoutType(uint a_uiLayoutType)
{
    m_Preferences.setLayoutType(a_uiLayoutType);
    save();
    emit layoutTypeChanged(a_uiLayoutType);
}

uint PreferenceService::getPlatformSortingCriteria() const
{
    return m_Preferences.getPlatformSortingCriteria();
}

void PreferenceService::setPlatformSortingCriteria(uint a_uiIndexCriteria)
{
    m_Preferences.setPlatformSortingCriteria(a_uiIndexCriteria);
    save();
    emit platformSortingCriteriaChanged(a_uiIndexCriteria);
}


//====================================================================================
// Operations
//====================================================================================

void PreferenceService::read(const QJsonObject& a_json)
{
    m_Preferences.read(a_json);
}

void PreferenceService::write(QJsonObject& a_json) const
{
    m_Preferences.write(a_json);
}

void PreferenceService::save() const
{
    // Write preferences json file
    QFile saveFile(QApplication::applicationDirPath() + "/preferences.json");

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open or create preferences file.");
        return;
    }

    QJsonObject jsonObject;
    write(jsonObject);
    QJsonDocument saveDoc(jsonObject);
    saveFile.write(saveDoc.toJson());
}
