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
#include <QUndoStack>
#ifdef _DEBUG
#include <QUndoView>
#endif
#include "sources/utils/tick.h"
#include "sources/database/platform.h"
#include "sources/database/collection.h"
#include "sources/database/database.h"
#include "sources/commands/commandmanager.h"
#include "platformwidget.h"
#include "preferences.h"
#include "preferenceswindows.h"
#include "style.h"

// ArcadeRoom version
#define ARCADE_ROOM_VERSION "v0.3"

// Tooltip content for tools bar buttons of game tab
#define TOOLTIP_GAME_TAB_TOOLS_BAR_COVER    "Display no cover rom"
#define TOOLTIP_GAME_TAB_TOOLS_BAR_NO_COVER "Do not display no cover rom"

// Criteria for console sorting in tree view
#define CRITERIA_CONSTRUCTOR      QT_TRANSLATE_NOOP("CRITERIA_PLATFORM","Constructor")
#define CRITERIA_GENERATION       QT_TRANSLATE_NOOP("CRITERIA_PLATFORM","Generation")
#define CRITERIA_NAME             QT_TRANSLATE_NOOP("CRITERIA_PLATFORM","Name")
#define MAX_CHAR_PER_CRITERIA     12

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
    // Enumerations
    //====================================================================================

    enum EGameLayoutType
    {
        GridWith3Columns = 0,
        GridWith4Columns = 1,
        GridWith5Columns = 2,
        Horizontal = 3,
        List = 4
    };


    //====================================================================================
    // Constructors
    //====================================================================================

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    //====================================================================================
    // Slots TreeView
    //====================================================================================

    /***********************************************************
     * @brief Called when a tree's item is clicked.
     *        Change the current platform.
     ***********************************************************/
    void                        on_treeWidget_clicked();

    /***********************************************************
     * @brief Called when the search button in treeview is pressed.
     *        Search a platform.
     ***********************************************************/
    void                        on_treeSearch_returnPressed();

    /***********************************************************
     * @brief Called when the current tab changed.
     *        Refresh the grid layout.
     * @param a_iIndex : current tab index
     ***********************************************************/
    void                        on_tabWidget_currentChanged(int a_iIndex);


    //====================================================================================
    // Slots GridLayout
    //====================================================================================

    /***********************************************************
     * @brief Called when a rom is clicked.
     *        Run the clicked rom.
     ***********************************************************/
    void                        on_button_clicked();

    /***********************************************************
     * @brief Called when the search button in grid is pressed.
     *        Search a game.
     ***********************************************************/
    void                        on_gridSearch_returnPressed();

    /***********************************************************
     * @brief Called when right click on game.
     *        Open the context menu.
     * @param point : click position
     ***********************************************************/
    void                        on_groupBox_customContextMenu(const QPoint& point);

    /***********************************************************
     * @brief Called when click on game.
     *        Run the game.
     * @param a_pTableItem : clicked item
     ***********************************************************/
    void                        on_gameListItem_clicked(QTableWidgetItem* a_pTableItem);


    //====================================================================================
    // Slots ListView
    //====================================================================================

    /***********************************************************
     * @brief Called when add collection button is clicked.
     *        Open a dialog box to create a new collection.
     ***********************************************************/
    void                        on_collectionAddButton_clicked();

    /***********************************************************
     * @brief Called when delete collection button is clicked.
     *        Delete the current selected collection.
     ***********************************************************/
    void                        on_collectionDeleteButton_clicked();

    /***********************************************************
     * @brief Called when a collection is clicked.
     *        Select the collection and display games.
     * @param a_pItem : selected item
     ***********************************************************/
    void                        on_collectionList_itemClicked(QListWidgetItem* a_pItem);

    /***********************************************************
     * @brief Called when the search collection button is pressed.
     *        Search a collection.
     ***********************************************************/
    void                        on_collectionSearch_returnPressed();


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
     * @brief Called when general preferences changed in preferences window.
     *        Save general preferences.
     ***********************************************************/
    void                        on_generalPreferencesChanged();

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
    // Slots ToolsBar
    //====================================================================================

    /***********************************************************
     * @brief Called when diplay no cover rom button is clicked.
     * @param a_bChecked : true if checked, false otherwise
     ***********************************************************/
    void                        on_buttonNoCover_clicked(bool a_bChecked);

    /***********************************************************
     * @brief Called when slider is pressed.
     *        Stop games loading.
     ***********************************************************/
    void                        on_horizontalSlider_sliderPressed();

    /***********************************************************
     * @brief Called when slider value is changed.
     * @param a_iValue : new slider value
     ***********************************************************/
    void                        on_horizontalSlider_valueChanged(int a_iValue);

    /***********************************************************
     * @brief Called when slider is released.
     *        Update icon sizes.
     ***********************************************************/
    void                        on_horizontalSlider_sliderReleased();

    /***********************************************************
     * @brief Called when layout type is changed.
     *        Update layout type.
     * @param a_iIndex : new layout type index
     ***********************************************************/
    void                        on_layoutTypeComboBox_currentIndexChanged(int a_iIndex);


    //====================================================================================
    // Slots Tick
    //====================================================================================

    /***********************************************************
     * @brief Called when tick.
     *        Add a game to grid layout.
     ***********************************************************/
    void                        on_tickTriggered();


    //====================================================================================
    // Slots Database
    //====================================================================================

    /***********************************************************
     * @brief Called when a collection is created.
     *        Add the collection to collection list widget.
     ***********************************************************/
    void                        on_collectionCreated(Collection* a_pCollection);

    /***********************************************************
     * @brief Called when a collection is deleted.
     *        Remove the collection from collection list widget.
     ***********************************************************/
    void                        on_collectionDeleted(Collection* a_pCollection);

    /***********************************************************
     * @brief Called when a game is added to collection.
     * @param a_pGame : added game
     ***********************************************************/
    void                        on_gameAddedToCollection(Game* a_pGame);

    /***********************************************************
     * @brief Called when a game is removed from collection.
     * @param a_pGame : removed game
     ***********************************************************/
    void                        on_gameRemovedFromCollection(Game* a_pGame);

    /***********************************************************
     * @brief Called when select a platform sorting criteria
     ***********************************************************/
    void                        on_comboBoxPlatformSorting_activated();


private:

    //====================================================================================
    // Private Operations
    //====================================================================================

    /***********************************************************
     * @brief Create the widget to display roms.
     ***********************************************************/
    void                        _createGameListWidget();

    /***********************************************************
     * @brief Delete the widget to display roms.
     ***********************************************************/
    void                        _deleteGameListWidget();

    /***********************************************************
     * @brief Load the general preferences (style, language, ...).
     ***********************************************************/
    void                        _loadGeneralPreferences();

    /***********************************************************
     * @brief Clear the grid layout.
     ***********************************************************/
    void                        _clearGridLayout();

    /***********************************************************
     * @brief Refresh the grid layout with given collection.
     * @param a_pCollection : collection to display
     ***********************************************************/
    void                        _refreshGridLayout(Collection* a_pCollection);

    /***********************************************************
     * @brief Refresh the grid layout with given platform.
     * @param a_pPlatform : platform to display
     ***********************************************************/
    void                        _refreshGridLayout(Platform* a_pPlatform);

    /***********************************************************
     * @brief Refresh the grid layout.
     ***********************************************************/
    void                        _refreshGridLayout();

    /***********************************************************
     * @brief Refresh the rom list for the current platform.
     * @param a_sPlatformName : platform's name
     ***********************************************************/
    void                        _loadGamesFromDirectory(const QString& a_sPlatformName);

    /***********************************************************
     * @brief Parse the rom list from given platform directory.
     * @param a_pPlatform : platform
     ***********************************************************/
    void                        _parseGamesFromDirectory(Platform* a_pPlatform);

    /***********************************************************
     * @brief Apply a style to given button.
     * @param button
     * @param groupBox
     * @param a_pPlatform : platform
     ***********************************************************/
    void                        _applyButtonStyle(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform);

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
     * @brief Set the icon size of given button.
     * @param button
     * @param groupBox
     * @param a_pPlatform : platform
     ***********************************************************/
    void                        _setIconSize(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform);

    /***********************************************************
     * @brief Run the given emulator with the given rom.
     * @param a_pGame : game to run
     ***********************************************************/
    void                        _runGame(Game* a_pGame);

    /***********************************************************
     * @brief Create a new collection.
     * @param a_sName : collection name
     ***********************************************************/
    void                        _createCollection(QString a_sName);

    /***********************************************************
     * @brief Delete the given collection.
     * @param a_sName : collection name
     ***********************************************************/
    void                        _deleteCollection(QString a_sName);

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
     * @brief Fill the tree view with loaded platforms.
     ***********************************************************/
    void                        _fillTreeView();

    /***********************************************************
     * @brief Set the selected item in treeview.
     * @param a_sPlatformName : selected platform name.
     ***********************************************************/
    void                        _setCurrentTreeViewItem(const QString& a_sPlatformName);

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
     * @brief Create a group box for a game.
     * @param a_pGame : game
     * @param a_bIsCoverExists : true if a game cover exists
     * @param a_sCurrentPath : current path of game
     * @param a_sCoverFiles : cover list
     * @param a_sRomFile : game file name
     ***********************************************************/
    QGroupBox*                  _createGameGroupBox(Game* a_pGame, bool a_bIsCoverExists, QString a_sCurrentPath,
                                                     QStringList a_sCoverFiles, QString a_sRomFile);

    /***********************************************************
     * @brief Load game metadatas (rating).
     ***********************************************************/
    void                        _loadMetadatas();

    /***********************************************************
     * @brief Save game metadatas (rating).
     ***********************************************************/
    void                        _saveMetadatas();

#ifdef _DEBUG
    /***********************************************************
     * @brief Create a view to display undo commands.
     ***********************************************************/
    void                        _createUndoView();
#endif


    //====================================================================================
    // Fields
    //====================================================================================

    // Main window of the application
    Ui::MainWindow*             m_pUI;

    // Database which contains platforms, games and collections
    Database                    m_Database;

    // Tick each N ms to add games asynchronously
    Tick*                       m_pTick;

    // Current selection (ex: Nes, Super Nes, Genesis, Favoris, ...)
    QString                     m_sCurrentPlatform;
    QString                     m_sCurrentCollection;

    // General base path to resources (emulators, games, ...)
    QString                     m_sResourcePath;

    // Current game directories used to display games
    QList<Game*>                m_Games;
    QList<Game*>                m_FilteredGames;
    int                         m_iCurrentGameCount;
    int                         m_iCurrentGamePosition;

    // Widget to display platform informations
    PlatformWidget*             m_pPlatformWidget;

    // Game layout to display games
    QWidget*                    m_pCurrentGameWidget;
    QLayout*                    m_pCurrentGameLayout;
    float                       m_fSizeFactor;
    MainWindow::EGameLayoutType m_eGameLayoutType;

    // Application preferences
    Preferences*                m_pGeneralPreferences;
    PreferencesWindows*         m_pPreferencesWindow;

    // Undo stack
    CommandManager*             m_pCommandManager;
    QUndoStack*                 m_pUndoStack;

    // Translation
    QTranslator                 m_Translator;

    // Style
    Style*                      m_pCurrentStyle;

#if _DEBUG
    QUndoView*                  m_pUndoView;
#endif
};

#endif // MAINWINDOW_H
