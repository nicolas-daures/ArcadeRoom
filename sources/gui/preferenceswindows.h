#ifndef PREFERENCESWINDOWS_H
#define PREFERENCESWINDOWS_H

#include <QDialog>
#include <QAbstractButton>
#include "sources/database/database.h"
#include "preferences.h"

namespace Ui
{
    class PreferencesWindows;
}

class PreferencesWindows : public QDialog
{
    Q_OBJECT

    // Platform preferences
    struct preferencesPlatform
    {
        // Emulator preferences
        QString emulatorPath;
        bool    emulatorPathToSave;

        // Roms preferences
        QString romsPath;
        bool    romsPathToSave;
    };

public:

    //====================================================================================
    // Constructors
    //====================================================================================

    explicit PreferencesWindows(QWidget *parent = 0,
                                Preferences* a_pGeneralPreferences = NULL);
    ~PreferencesWindows();

    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Translate window content.
     ***********************************************************/
    void retranslate();

    /***********************************************************
     * @brief Update window content.
     * @param a_Database   : database
     ***********************************************************/
    void updateContent(const Database& a_Database);


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Sent when an emulator path is changed in
     *        preferences window.
     * @param emulatorPath : emulator path
     * @param PlatformName : platform name on which emulator
     *                       path has changed
     ***********************************************************/
    void emulatorPathChanged(QString emulatorPath, QString plaformName);

    /***********************************************************
     * @brief Sent when a roms path is changed in preferences
     *        window.
     * @param romsPath     : roms path
     * @param PlatformName : platform name on which roms
     *                       path has changed
     ***********************************************************/
    void romsPathChanged(QString romsPath, QString plaformName);

    /***********************************************************
     * @brief Sent when "show no covers" checkbox state is changed
     *        in preferences window.
     * @param a_bShowNoCovers : true if show no covers
     ***********************************************************/
    void showNoCoversChanged(bool a_bShowNoCovers);

    /***********************************************************
     * @brief Sent when style is changed in preferences window.
     * @param a_sStyleName : new style name
     ***********************************************************/
    void styleNameChanged(QString a_sStyleName);

    /***********************************************************
     * @brief Sent when general preferences is changed in
     *        preferences window.
     ***********************************************************/
    void generalPreferencesChanged();


private slots:

    //====================================================================================
    // Slots
    //====================================================================================

    /***********************************************************
     * @brief Called when browse emulator path.
     *        Open a dialog box to choose emulator executable.
     ***********************************************************/
    void on_pushButtonBrowseEmulatorPath_clicked();

    /***********************************************************
     * @brief Called when browse roms path.
     *        Open a dialog box to choose roms directory.
     ***********************************************************/
    void on_pushButtonBrowseRomsPath_clicked();

    /***********************************************************
     * @brief Called when select a new platform in platform list.
     *        Display last saved emulator path in matching text
     *        area.
     ***********************************************************/
    void on_comboBoxPlatformsList_activated(const QString &arg1);

    /***********************************************************
     * @brief Called when clicked on apply button of the window.
     *        Apply changes by sending signals.
     ***********************************************************/
    void on_buttonApply_clicked();

    /***********************************************************
     * @brief Called when clicked on close button of the window.
     *        Close the preferences windows.
     ***********************************************************/
    void on_buttonClose_clicked();

    /***********************************************************
     * @brief Called when clicked on the checkbox to show no covers.
     * @param a_iNewState : new state of the checkbox
     ***********************************************************/
    void on_showNoCoversCheckBox_stateChanged(int a_iNewState);

    /***********************************************************
     * @brief Called when style changed.
     * @param a_sStyleName : new style
     ***********************************************************/
    void on_styleComboBox_currentIndexChanged(const QString& a_sStyleName);

    /***********************************************************
     * @brief Called when language changed.
     * @param a_sStyleName : new language
     ***********************************************************/
    void on_languageComboBox_currentIndexChanged(const QString& a_sLanguage);


private:

    //====================================================================================
    // Private Operations
    //====================================================================================

    /***********************************************************
     * @brief Load styles from disk.
     ***********************************************************/
    void _loadStyles();

    /***********************************************************
     * @brief Update general preferences content.
     * @param a_Database   : database
     ***********************************************************/
    void _updateGeneral();

    /***********************************************************
     * @brief Update platform content.
     * @param a_Database   : database
     ***********************************************************/
    void _updatePlatform(const Database& a_Database);

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::PreferencesWindows*     ui;

    // Platform name <-> Platform preferences
    QMap<QString, preferencesPlatform> m_PlatformPreferencesMap;

    // Preferences
    Preferences*                m_pGeneralPreferences;
    Preferences                 m_CurrentPreferences;
};

#endif // PREFERENCESWINDOWS_H
