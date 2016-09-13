#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QTranslator>
#include <QGroupBox>
#include <QVector>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include "sources/utils/tick.h"
#include "sources/database/platform.h"
#include "sources/database/collection.h"
#include "sources/database/database.h"
#include "sources/commands/commandservice.h"
#include "sources/services/selectionservice.h"
#include "platformlistwidget.h"
#include "collectionlistwidget.h"
#include "gamelistwidget.h"
#include "platformwidget.h"
#include "preferenceservice.h"
#include "preferenceswindows.h"
#include "style.h"

// ArcadeRoom version
#define ARCADE_ROOM_VERSION "v0.4.0"

// Criteria for console sorting in tree view
// Langage
#define DEFAULT_LANGUAGE "en_english"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create the main window of the application.
     * @param a_pParent : parent widget
     ***********************************************************/
    explicit MainWindow(QWidget* a_pParent = 0);
    ~MainWindow();


private slots:

    //====================================================================================
    // Slots
    //====================================================================================

    /***********************************************************
     * @brief Called when the current platform changed.
     *        Refresh the grid layout and console overview.
     * @param a_sPlatform : selected platform
     ***********************************************************/
    void                        on_platformSelected(QString a_sPlatform);

    /***********************************************************
     * @brief Called when the current collection changed.
     *        Refresh the grid layout.
     * @param a_sCollection : selected collection
     ***********************************************************/
    void                        on_collectionSelected(QString a_sCollection);

    /***********************************************************
     * @brief Called when a collection is created.
     ***********************************************************/
    void                        on_collectionCreated(Collection* a_pCollection);

    /***********************************************************
     * @brief Called when a game is remove from collection.
     *        Refresh the grid layout.
     ***********************************************************/
    void                        on_gameRemovedFromCollection(Game* a_pGame);

    /***********************************************************
     * @brief Called when the current tab changed.
     *        Refresh the grid layout.
     * @param a_iIndex : current tab index
     ***********************************************************/
    void                        on_tabWidget_currentChanged(int a_iIndex);


    //====================================================================================
    // Slots MenuBar
    //====================================================================================

    /***********************************************************
     * @brief Called when the preferences action is triggered.
     *        Open a dialog to choose a directory.
     ***********************************************************/
    void                        on_actionPreferences_triggered();

    /***********************************************************
     * @brief Called when the about action is triggered.
     *        Open documentation website in default browser.
     ***********************************************************/
    void                        on_actionDocumentation_triggered();

    /***********************************************************
     * @brief Called when the website action is triggered.
     *        Open ArcadeRoom website in default browser.
     ***********************************************************/
    void                        on_actionWebsite_triggered();

    /***********************************************************
     * @brief Called when the about action is triggered.
     *        Open a dialog which contains ArcadeRoom informations.
     ***********************************************************/
    void                        on_actionAbout_triggered();

    /***********************************************************
     * @brief Called when the quit action is triggered.
     *        Quit the application.
     ***********************************************************/
    void                        on_actionQuit_triggered();

    /***********************************************************
     * @brief Called when the undo action is triggered.
     *        Undo the last action.
     ***********************************************************/
    void                        on_actionUndo_triggered();

    /***********************************************************
     * @brief Called when the redo action is triggered.
     *        Redo the last undo action.
     ***********************************************************/
    void                        on_actionRedo_triggered();

    /***********************************************************
     * @brief Called when language changed in preferences window.
     *        Change application language.
     * @param a_sLanguage : new language.
     ***********************************************************/
    void                        on_languageChanged(QString a_sLanguage);

    /***********************************************************
     * @brief Called when style name changed in preferences window.
     *        Change application style.
     * @param a_sStyleName : name of the new style.
     ***********************************************************/
    void                        on_styleNameChanged(QString a_sStyleName);

    /***********************************************************
     * @brief Called when action to change console visibility changed.
     *        Change console/collection visibility.
     * @param a_bIsChecked : true if checkbox is checked
     ***********************************************************/
    void                        on_actionConsoles_toggled(bool a_bIsChecked);

    /***********************************************************
     * @brief Called when action to change tools bar visibility changed.
     *        Change tools bar visibility.
     * @param a_bIsChecked : true if checkbox is checked
     ***********************************************************/
    void                        on_actionTools_Bar_toggled(bool a_bIsChecked);

    /***********************************************************
     * @brief Called when action to change status bar visibility changed.
     *        Change status bar visibility.
     * @param a_bIsChecked : true if checkbox is checked
     ***********************************************************/
    void                        on_actionStatus_Bar_toggled(bool a_bIsChecked);


    //====================================================================================
    // Slots Database
    //====================================================================================

    /***********************************************************
     * @brief Called when game parameter changed.
     *        Refresh grid layout.
     ***********************************************************/
    void                        on_gameParameterChanged();


private:

    //====================================================================================
    // Private Operations
    //====================================================================================

    /***********************************************************
     * @brief Refresh the grid layout with given collection.
     * @param a_sCollectionName : name of collection to display
     ***********************************************************/
    void                        _refreshCollectionGridLayout(const QString& a_sCollectionName);

    /***********************************************************
     * @brief Refresh the grid layout with given platform.
     * @param a_sPlatformName : name of platform to display
     ***********************************************************/
    void                        _refreshPlatformGridLayout(const QString& a_sPlatformName);

    /***********************************************************
     * @brief Refresh the grid layout.
     ***********************************************************/
    void                        _refreshGridLayout();

    /***********************************************************
     * @brief Load the general preferences (style, language, ...).
     ***********************************************************/
    void                        _loadGeneralPreferences();

    /***********************************************************
     * @brief Parse the rom list from given platform directory.
     * @param a_pPlatform : platform
     ***********************************************************/
    void                        _parseGamesFromDirectory(Platform* a_pPlatform);

    /***********************************************************
     * @brief Get the path of emulator.
     * @return emulator path
     ***********************************************************/
    QString                     _getEmulatorPath();

    /***********************************************************
     * @brief Get the rom extension filter.
     * @param a_pPlatform : platform
     * @return rom filter
     ***********************************************************/
    QStringList                 _getRomFilter(Platform* a_pPlatform);

    /***********************************************************
     * @brief Set the language of the application.
     * @param a_sLanguage : language
     ***********************************************************/
    void                        _setLanguage(QString a_sLanguage);

    /***********************************************************
     * @brief Set the style of the application.
     * @param a_sStyleName : style name
     ***********************************************************/
    void                        _setStyle(QString a_sStyleName);

    /***********************************************************
     * @brief Update the style of the icons.
     ***********************************************************/
    void                        _updateIconsStyle();

    /***********************************************************
     * @brief Update the style of the widgets.
     ***********************************************************/
    void                        _updateWidgetsStyle();

    /***********************************************************
     * @brief Delete the game with given platform and name.
     * @param a_sPlatformName : platform's name.
     * @param a_sGameName : game's name.
     ***********************************************************/
    void                        _deleteGame(const QString& a_sPlatformName, const QString& a_sGameName);

    /***********************************************************
     * @brief Add the given game to given collection.
     * @param a_pCollection : collection.
     * @param a_pGame : game to add.
     ***********************************************************/
    void                        _addGameToCollection(Collection* a_pCollection, Game* a_pGame);

    /***********************************************************
     * @brief Remove the given game from given collection.
     * @param a_pCollection : collection.
     * @param a_pGame : game to remove.
     ***********************************************************/
    void                        _removeGameFromCollection(Collection* a_pCollection, Game* a_pGame);

    /***********************************************************
     * @brief Refresh the panel displaying platform informations.
     ***********************************************************/
    void                        _refreshPlatformPanel();

    /***********************************************************
     * @brief Refresh the status bar informations.
     ***********************************************************/
    void                        _refreshStatusBar();

    /***********************************************************
     * @brief Refresh the sorting platforms combobox.
     ***********************************************************/
    void                        _refreshComboBoxPlatformSorting();

    /***********************************************************
     * @brief Load game metadatas (rating).
     ***********************************************************/
    void                        _loadMetadatas();


    //====================================================================================
    // Fields
    //====================================================================================

    // Main window of the application
    Ui::MainWindow*             m_pUI;

    // Application directory
    QString                     m_sAppDirectory;

    // Tick each N ms to add games asynchronously
    Tick*                       m_pTick;

    // General base path to resources (emulators, games, ...)
    QString                     m_sResourcePath;

    // Widget to display platform list
    PlatformListWidget*         m_pPlatformListWidget;

    // Widget to display platform informations
    PlatformWidget*             m_pPlatformWidget;

    // Widget to display collection list
    CollectionListWidget*       m_pCollectionListWidget;

    // Widget to display game list
    GameListWidget*             m_pGameListWidget;

    // Application preferences
    PreferencesWindows*         m_pPreferencesWindow;

    // Translation
    QTranslator                 m_Translator;

    // Style
    Style*                      m_pCurrentStyle;
};

#endif // MAINWINDOW_H
