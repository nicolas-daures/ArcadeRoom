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
#include "sources/database/databaseservice.h"
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

    /***********************************************************
     * @brief Game layout types (ex: grid, list, ...).
     ***********************************************************/
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

    /***********************************************************
     * @brief Create a widget to show game list.
     * @param a_pParent : parent widget
     ***********************************************************/
    explicit GameListWidget(QWidget* a_pParent = NULL);
    ~GameListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    void                            setToolBarVisibility(bool a_bVisibility);
    void                            setButtonNoCoverToolTip(QString a_sTooltip);
    void                            setButtonNoCoverChecked(bool a_bChecked);
    void                            setHorizontalSliderValue(int a_iSliderValue);

    int                             getCurrentLayoutType();
    void                            setCurrentLayoutType(int a_iLayoutType);


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when a game parameter changed.
     ***********************************************************/
    void                            gameParameterChanged();

    /***********************************************************
     * @brief Called when a game is displayed.
     ***********************************************************/
    void                            gameDisplayed();


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
    void                            on_buttonClicked();

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
    void                            on_gameListItemClicked(QTableWidgetItem* a_pTableItem);

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
    void                            on_groupBoxCustomContextMenu(const QPoint& point);



public:

    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Start the game refreshing.
     ***********************************************************/
    void                            start();

    /***********************************************************
     * @brief Stop the game refreshing.
     ***********************************************************/
    void                            stop();

    /***********************************************************
     * @brief Update the style of the icons.
     * @param a_sIconPath : path of the icons
     ***********************************************************/
    void                            updateIconsStyle(QString a_sIconPath);

    /***********************************************************
     * @brief Clear the grid layout.
     ***********************************************************/
    void                            clearGridLayout();

    /***********************************************************
     * @brief Create the widget to display roms.
     ***********************************************************/
    void                            createGameListWidget(QWidget* a_pParent);

    /***********************************************************
     * @brief Update the style of the widgets.
     ***********************************************************/
    void                            updateWidgetsStyle();


private:

    //====================================================================================
    // Private Operations
    //====================================================================================

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

    /***********************************************************
     * @brief Delete the widget to display roms.
     ***********************************************************/
    void                            _deleteGameListWidget();

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


    //====================================================================================
    // Fields
    //====================================================================================

    Ui::GameListWidget*             m_pUI;

    QWidget*                        m_pParentWidget;

    int                             m_iCurrentGameCount;
    int                             m_iCurrentGamePosition;

    // Game layout to display games
    QWidget*                        m_pCurrentGameWidget;
    QLayout*                        m_pCurrentGameLayout;
    float                           m_fSizeFactor;
    GameListWidget::EGameLayoutType m_eGameLayoutType;
};

#endif // GAMELISTWIDGET_H
