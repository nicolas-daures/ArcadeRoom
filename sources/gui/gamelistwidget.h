#ifndef GAMELISTWIDGET_H
#define GAMELISTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSlider>
#include <QComboBox>
#include <QLabel>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QToolButton>
#include "sources/database/database.h"
#include "sources/database/platform.h"
#include "sources/database/collection.h"
#include "style.h"
#include "preferenceservice.h"

// Tooltip content for tools bar buttons of game tab
#define TOOLTIP_GAME_TAB_TOOLS_BAR_COVER    "Display no cover rom"
#define TOOLTIP_GAME_TAB_TOOLS_BAR_NO_COVER "Do not display no cover rom"

namespace Ui
{
    class GameListWidget;
}

class GameListWidget : public QWidget
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

    explicit GameListWidget(Database* a_pDatabase, QWidget* a_pParent = NULL);
    ~GameListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    // TODO : remove it
    QWidget*                        getGamesToolBar();
    QPushButton*                    getButtonNoCover();
    QVBoxLayout*                    getGameListContainerLayout();
    QSlider*                        getHorizontalSlider();
    QComboBox*                      getLayoutTypeComboBox();
    QLabel*                         getCoverSizeIcon();

    QList<Game*>                    getGames();
    void                            setGames(QList<Game*> a_Games);
    void                            setFilteredGames(QList<Game*> a_Games);


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when a game parameter changed.
     ***********************************************************/
    void                            gameParameterChanged();


public slots:

    //====================================================================================
    // Slots
    //====================================================================================

    /***********************************************************
     * @brief Called when diplay no cover rom button is clicked.
     * @param a_bChecked : true if checked, false otherwise
     ***********************************************************/
    void                            on_buttonNoCover_clicked(bool a_bChecked);

    /***********************************************************
     * @brief Called when slider is pressed.
     *        Stop games loading.
     ***********************************************************/
    void                            on_horizontalSlider_sliderPressed();

    /***********************************************************
     * @brief Called when slider value is changed.
     * @param a_iValue : new slider value
     ***********************************************************/
    void                            on_horizontalSlider_valueChanged(int a_iValue);

    /***********************************************************
     * @brief Called when slider is released.
     *        Update icon sizes.
     ***********************************************************/
    void                            on_horizontalSlider_sliderReleased();

    /***********************************************************
     * @brief Called when layout type is changed.
     *        Update layout type.
     * @param a_iIndex : new layout type index
     ***********************************************************/
    void                            on_layoutTypeComboBox_currentIndexChanged(int a_iIndex);

    /***********************************************************
     * @brief Called when a rom is clicked.
     *        Run the clicked rom.
     ***********************************************************/
    void                            on_button_clicked();

    /***********************************************************
     * @brief Called when the search button in grid is pressed.
     *        Search a game.
     ***********************************************************/
    void                            on_gridSearch_returnPressed();

    /***********************************************************
     * @brief Called when click on game.
     *        Run the game.
     * @param a_pTableItem : clicked item
     ***********************************************************/
    void                            on_gameListItem_clicked(QTableWidgetItem* a_pTableItem);

    /***********************************************************
     * @brief Called when tick.
     *        Add a game to grid layout.
     ***********************************************************/
    void                            on_tickTriggered();

    /***********************************************************
     * @brief Called when right click on game.
     *        Open the context menu.
     * @param point : click position
     ***********************************************************/
    void                            on_groupBox_customContextMenu(const QPoint& point);



public:

    //====================================================================================
    // Operations
    //====================================================================================

    void                            start();
    void                            stop();

    /***********************************************************
     * @brief Create a group box for a game.
     * @param a_pGame : game
     * @param a_bIsCoverExists : true if a game cover exists
     * @param a_sCurrentPath : current path of game
     * @param a_sCoverFiles : cover list
     * @param a_sRomFile : game file name
     ***********************************************************/
    QGroupBox*                      _createGameGroupBox(Game* a_pGame, bool a_bIsCoverExists, QString a_sCurrentPath,
                                                     QStringList a_sCoverFiles, QString a_sRomFile);


// TODO : private

    //====================================================================================
    // Private Operations
    //====================================================================================

    /***********************************************************
     * @brief Clear the grid layout.
     ***********************************************************/
    void                            _clearGridLayout();


    /***********************************************************
     * @brief Create the widget to display roms.
     ***********************************************************/
    void                            _createGameListWidget(QWidget* a_pParent);

    /***********************************************************
     * @brief Delete the widget to display roms.
     ***********************************************************/
    void                            _deleteGameListWidget();

    /***********************************************************
     * @brief Update the style of the widgets.
     * @param a_pStyle : style to apply
     ***********************************************************/
    void                            _updateWidgetsStyle(Style* a_pStyle);

    /***********************************************************
     * @brief Apply a style to given button.
     * @param button
     * @param groupBox
     * @param a_pPlatform : platform
     ***********************************************************/
    void                            _applyButtonStyle(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform);

    /***********************************************************
     * @brief Set the icon size of given button.
     * @param button
     * @param groupBox
     * @param a_pPlatform : platform
     ***********************************************************/
    void                            _setIconSize(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform);

    /***********************************************************
     * @brief Run the given emulator with the given rom.
     * @param a_pGame : game to run
     ***********************************************************/
    void                            _runGame(Game* a_pGame);

    /***********************************************************
     * @brief Save game metadatas (rating).
     ***********************************************************/
    void                            _saveMetadatas();


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::GameListWidget*             m_pUI;

    Database*                       m_pDatabase;

    QWidget*                        m_pParentWidget;

    // Current game directories used to display games
    QList<Game*>                    m_Games;

    int                             m_iCurrentGameCount;
    int                             m_iCurrentGamePosition;

    // Game layout to display games
    QWidget*                        m_pCurrentGameWidget;
    QLayout*                        m_pCurrentGameLayout;
    float                           m_fSizeFactor;
    GameListWidget::EGameLayoutType m_eGameLayoutType;

    QList<Game*>                    m_FilteredGames;
};

#endif // GAMELISTWIDGET_H
