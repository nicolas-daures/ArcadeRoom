#ifndef PREFERENCESERVICE_H
#define PREFERENCESERVICE_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include "preferences.h"

class PreferenceService : public QObject
{
    Q_OBJECT


private:

    //====================================================================================
    // Constructors
    //====================================================================================

    PreferenceService();


public:

    //====================================================================================
    // Accessors
    //====================================================================================

    static PreferenceService*   getInstance();

    const QString&              getLanguage() const;
    void                        setLanguage(const QString& a_sLanguage);

    const QString&              getStyleName() const;
    void                        setStyleName(const QString& a_sStyleName);

    bool                        getShowNoCovers() const;
    void                        setShowNoCovers(bool a_bShowNoCovers);

    bool                        getShowConsoles() const;
    void                        setShowConsoles(bool a_bShowConsoles);

    bool                        getShowToolsBar() const;
    void                        setShowToolsBar(bool a_bShowToolsBar);

    bool                        getShowStatusBar() const;
    void                        setShowStatusBar(bool a_bShowStatusBar);

    float                       getCoverSizeFactor() const;
    void                        setCoverSizeFactor(float a_fCoverSizeFactor);

    uint                        getLayoutType() const;
    void                        setLayoutType(uint a_uiLayoutType);

    uint                        getPlatformSortingCriteria() const;
    void                        setPlatformSortingCriteria(uint a_iIndexCriteria);


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Deserialize the given JSON object.
     * @param a_json : JSON object to deserialize
     ***********************************************************/
    void                        read(const QJsonObject& a_json);

    /***********************************************************
     * @brief Serialize this in the given JSON object.
     * @param a_json : JSON object
     ***********************************************************/
    void                        write(QJsonObject& a_json) const;

    /***********************************************************
     * @brief Save in preferences file.
     ***********************************************************/
    void                        save() const;


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when the language changed.
     * @param a_sLanguage : new language
     ***********************************************************/
    void                        languageChanged(QString a_sLanguage);

    /***********************************************************
     * @brief Called when the style's name changed.
     * @param a_sStyleName : new style's name
     ***********************************************************/
    void                        styleNameChanged(QString a_sStyleName);

    /***********************************************************
     * @brief Called when the "show no covers" boolean changed.
     * @param a_bShowNoCovers : new "show no covers" boolean
     ***********************************************************/
    void                        showNoCoversChanged(bool a_bShowNoCovers);

    /***********************************************************
     * @brief Called when the "show consoles" boolean changed.
     * @param a_bShowConsoles : new "show consoles" boolean
     ***********************************************************/
    void                        showConsolesChanged(bool a_bShowConsoles);

    /***********************************************************
     * @brief Called when the "show tools bar" boolean changed.
     * @param a_bShowToolsBar : new "show tools bar" boolean
     ***********************************************************/
    void                        showToolsBarChanged(bool a_bShowToolsBar);

    /***********************************************************
     * @brief Called when the "show status bar" boolean changed.
     * @param a_bShowStatusBar : new "show status bar" boolean
     ***********************************************************/
    void                        showStatusBarChanged(bool a_bShowStatusBar);

    /***********************************************************
     * @brief Called when the cover size factor changed.
     * @param a_fCoverSizeFactor : new cover size factor
     ***********************************************************/
    void                        coverSizeFactorChanged(float a_fCoverSizeFactor);

    /***********************************************************
     * @brief Called when the layout type changed.
     * @param a_uiLayoutType : new layout type
     ***********************************************************/
    void                        layoutTypeChanged(uint a_uiLayoutType);

    /***********************************************************
     * @brief Called when the platform sorting criteria changed.
     * @param a_iIndexCriteria : index in combobox of new criteria
     ***********************************************************/
    void                        platformSortingCriteriaChanged(uint a_iIndexCriteria);


private:

    //====================================================================================
    // Fields
    //====================================================================================

    static PreferenceService*   m_pInstance;

    Preferences                 m_Preferences;
};

#endif // PREFERENCESERVICE_H
