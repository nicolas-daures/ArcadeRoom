#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QStyleFactory>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMessageBox>
#include <QFile>
#include <QInputDialog>
#include <QJsonDocument>
#include <QWebView>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QScrollArea>
#include <QJsonArray>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "preferenceswindows.h"
#include "style.h"
#include "sources/database/platform.h"
#include "sources/database/collection.h"
#include "sources/commands/createcollectioncommand.h"
#include "sources/commands/deletecollectioncommand.h"
#include "sources/commands/addgametocollectioncommand.h"
#include "sources/commands/removegamefromcollectioncommand.h"
#ifdef WIN32
#include <Windows.h>
#include <qset.h>
#endif //WIN32


//====================================================================================
// Constructors
//====================================================================================

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_pUI(new Ui::MainWindow),
    m_sAppDirectory(QApplication::applicationDirPath()),
    m_sResourcePath(QApplication::applicationDirPath() + "/games"),
    m_iCurrentGameCount(-1),
    m_iCurrentGamePosition(0),
    m_pPlatformWidget(NULL),
    m_pCurrentGameWidget(NULL),
    m_pCurrentGameLayout(NULL),
    m_fSizeFactor(1.0f),
    m_eGameLayoutType(GridWith3Columns),
    m_pCurrentStyle(NULL)
{
    // Create undo stack
    m_pCommandManager = new CommandManager(&m_Database);
    m_pUndoStack = new QUndoStack(this);
#ifdef _DEBUG
    _createUndoView();
#endif
    // Load platforms
    m_Database.loadPlatforms();

    // Load games
    foreach (Platform* pPlatform, m_Database.getPlatforms())
    {
        _parseGamesFromDirectory(pPlatform->getName());
    }

    // Load metadatas
    _loadMetadatas();

    // Initialize UI
    m_pUI->setupUi(this);

    // Load general preferences
    _loadGeneralPreferences();

    // Create preferences window
    m_pPreferencesWindow = new PreferencesWindows(this, m_pGeneralPreferences);

    // Translate application if requiered
    _setLanguage(m_pGeneralPreferences->getLanguage());

    // Signal when general preferences updated
    connect(m_pGeneralPreferences, SIGNAL(styleNameChanged(QString)), this, SLOT(on_styleNameChanged(QString)));
    connect(m_pGeneralPreferences, SIGNAL(languageChanged(QString)),  this, SLOT(on_languageChanged(QString)));
    connect(m_pPreferencesWindow, SIGNAL(emulatorPathChanged(QString, QString)), &m_Database, SLOT(on_emulatorPathChanged(QString, QString)));
    connect(m_pPreferencesWindow, SIGNAL(romsPathChanged(QString, QString)), &m_Database, SLOT(on_romsPathChanged(QString, QString)));
    connect(m_pPreferencesWindow, SIGNAL(generalPreferencesChanged()), this, SLOT(on_generalPreferencesChanged()));

    // Hide header of platform tree view
    m_pUI->treeWidget->header()->close();

    // Fill treeview
    _fillTreeView();

    // Connections
    connect(&m_Database, SIGNAL(collectionCreated(Collection*)), this, SLOT(on_collectionCreated(Collection*)));
    connect(&m_Database, SIGNAL(collectionDeleted(Collection*)), this, SLOT(on_collectionDeleted(Collection*)));

    // Create a platform panel
    QVBoxLayout* pPlatformLayout = new QVBoxLayout(m_pUI->tabConsole);
    m_pUI->tabConsole->setLayout(pPlatformLayout);
    m_pPlatformWidget = new PlatformWidget();
    pPlatformLayout->addWidget(m_pPlatformWidget);
    m_sCurrentPlatform = "Nes";
    m_sCurrentCollection = "";
    _refreshPlatformPanel();

    // Create a simple grid layout to display roms
    _createGameListWidget();
    _loadGamesFromDirectory(m_sCurrentPlatform);

    // Create a tick object to load rom covers asynchronously
    m_pTick = new Tick();
    connect(m_pTick, SIGNAL(ticked()), this, SLOT(on_tickTriggered()));
    m_pTick->start();

    // Load collection on disk
    m_Database.loadCollections();
    foreach (Collection* pCollection, m_Database.getCollections())
    {
        m_pUI->collectionList->addItem(pCollection->getName());
        connect(pCollection, SIGNAL(gameAdded(Game*)), this, SLOT(on_gameAddedToCollection(Game*)));
        connect(pCollection, SIGNAL(gameRemoved(Game*)), this, SLOT(on_gameRemovedFromCollection(Game*)));
    }
}

MainWindow::~MainWindow()
{
    delete m_pUI;
}


//====================================================================================
// Slots TreeView
//====================================================================================

void MainWindow::on_treeWidget_clicked()
{
    // Set the new platform and refresh the rom list
    QString newPlatform = m_pUI->treeWidget->currentItem()->text(m_pUI->treeWidget->currentColumn());
    if (newPlatform != m_sCurrentPlatform)
    {
        QList<Platform*> platforms = m_Database.getPlatforms();
        foreach (Platform* pPlatform, platforms)
        {
            if (pPlatform->getName() == newPlatform)
            {
                m_sCurrentCollection = "";
                m_sCurrentPlatform = pPlatform->getName();
                break;
            }
        }
        _refreshPlatformPanel();
        _loadGamesFromDirectory(m_sCurrentPlatform);
    }
}

void MainWindow::on_treeSearch_returnPressed()
{
    QLineEdit* pLineEdit = dynamic_cast<QLineEdit*>(sender());
    if(pLineEdit != NULL)
    {
        QString searchText = pLineEdit->text();
        QList<Platform*> platforms = m_Database.getPlatforms();

        // Search same name
        foreach (Platform* pPlatform, platforms)
        {
            if (pPlatform->getName().toLower() == searchText.toLower())
            {
                _setCurrentTreeViewItem(pPlatform->getName());
                return;
            }
        }

        // Search if contains
        foreach (Platform* pPlatform, platforms)
        {
            if (pPlatform->getName().toLower().contains(searchText.toLower()))
            {
                _setCurrentTreeViewItem(pPlatform->getName());
                return;
            }
        }
    }
}

void MainWindow::on_tabWidget_currentChanged(int a_iIndex)
{
    switch (a_iIndex)
    {
        case 0: // TreeView
        {
            QTreeWidgetItem* pItem = m_pUI->treeWidget->currentItem();
            if (pItem != NULL)
            {
                QString sPlatformName = pItem->text(0);
                Platform* pPlatform = m_Database.getPlatform(sPlatformName);
                if (pPlatform)
                {
                    _refreshGridLayout(pPlatform);
                }
            }
        }
        break;
        case 1: // ListView
        {
            QListWidgetItem* pItem = m_pUI->collectionList->currentItem();
            if (pItem != NULL)
            {
                QString sCollectionName = pItem->text();
                Collection* pCollection = m_Database.getCollection(sCollectionName);
                if (pCollection)
                {
                    _refreshGridLayout(pCollection);
                }
            }
        }
        break;
    default:
        break;
    }
}


//====================================================================================
// Slots GridLayout
//====================================================================================

void MainWindow::on_button_clicked()
{
    // Get the clicked rom
    QToolButton* button = dynamic_cast<QToolButton*>(sender());
    if( button != NULL )
    {
        QString sPlatformName = button->property("platform").toString();
        QString sGameName = button->property("game").toString();

        // Run emulator with rom
        Game* pGame = m_Database.getGame(sPlatformName, sGameName);
        _runGame(pGame);
    }
}

void MainWindow::on_gridSearch_returnPressed()
{
    QLineEdit* pLineEdit = dynamic_cast<QLineEdit*>(sender());
    if(pLineEdit != NULL)
    {
        // Search games
        m_iCurrentGameCount = -1;
        m_FilteredGames.clear();
        QString searchText = pLineEdit->text();
        for (int iDirIndex = 0; iDirIndex < m_Games.size(); ++iDirIndex)
        {
            QString dir = m_Games[iDirIndex]->getName();
            if (dir.toLower().contains(searchText.toLower()))
            {
                m_FilteredGames.push_back(m_Games[iDirIndex]);
            }
        }

        // Clear games
        _clearGridLayout();

        // When current game count >= 0, the tick works
        m_iCurrentGameCount = 0;
        m_iCurrentGamePosition = 0;
    }
}

void MainWindow::on_groupBox_customContextMenu(const QPoint &point)
{
    QGroupBox* groupBox = dynamic_cast<QGroupBox*>(sender());
    if (groupBox != NULL)
    {
        QPoint globalPos = groupBox->mapToGlobal(point);

        // Get the selected game
        QString sPlatformName = groupBox->property("platform").toString();
        QString sGameName = groupBox->property("game").toString();
        QMap<QString, QUndoCommand*> commands = m_pCommandManager->getGameCommands(sPlatformName, sGameName);

        // Create a context menu and and action to run a game
        QMenu contextMenu;
        contextMenu.addAction("Run the game");
        contextMenu.addSeparator();

        // Add actions to add/remove a game in a collection
        foreach (QString sCommandText, commands.keys())
        {
            contextMenu.addAction(sCommandText);
        }

        // Execute the context menu
        QAction* selectedItem = contextMenu.exec(globalPos);
        if (selectedItem != NULL)
        {
            if (commands[selectedItem->text()] != NULL)
            {
                m_pUndoStack->push(commands[selectedItem->text()]);
            }
            else
            {
                // Run the game
                Game* pGame = m_Database.getGame(sPlatformName, sGameName);
                if (pGame != NULL)
                {
                    _runGame(pGame);
                }
            }
        }
    }
}

void MainWindow::on_gameListItem_clicked(QTableWidgetItem* a_pTableItem)
{
    // Get the clicked game
    int iRowIndex = a_pTableItem->row();
    int iColumnIndex = a_pTableItem->column();
    QTableWidget* pTableWidget = dynamic_cast<QTableWidget*>(m_pCurrentGameWidget);
    QString sGameName = pTableWidget->item(iRowIndex, 0)->statusTip();//pTableWidget->item(iRowIndex, 0)->text();
    QString sPlatformName = pTableWidget->item(iRowIndex, 1)->text();
    Game* pGame = m_Database.getGame(sPlatformName, sGameName);

    if (iColumnIndex == 0)
    {
        // Run emulator with rom
        _runGame(pGame);
    }
    else if (iColumnIndex == 2)
    {
        // Update the note
        uint uiRating = pGame->getRating();
        pGame->setRating(uiRating % 5 + 1);
        QString starFilePath = QString(":/images/resources/images/rating_") + QString::number(pGame->getRating()) + QString(".png");
        a_pTableItem->setIcon(QIcon(starFilePath));
        _saveMetadatas();
    }
}


//====================================================================================
// Slots ListView
//====================================================================================

void MainWindow::on_collectionAddButton_clicked()
{
    bool bOk;

    QString sText = QInputDialog::getText(this, tr("Add a collection"),
                                         tr("Collection's name:"), QLineEdit::Normal,
                                         tr("Collection Name"), &bOk);

    if (bOk && !sText.isEmpty())
    {
        // Check if collection already exists
        Collection* pCollection = m_Database.getCollection(sText);
        if (pCollection == NULL)
        {
            _createCollection(sText);
        }
        else
        {
            // Display an error dialog
            QMessageBox::warning(this, tr("Warning"), tr("This collection already exists !"), QMessageBox::Ok);
        }
    }
}

void MainWindow::on_collectionDeleteButton_clicked()
{
    if (m_sCurrentCollection != "")
    {
        // Delete the collection
        _deleteCollection(m_sCurrentCollection);

        // Select another collection
        QList<Collection*> collections = m_Database.getCollections();
        if (collections.size() > 0)
        {
            Collection* pNewCollection = collections[0];
            QString sNewCollectionName = pNewCollection->getName();
            QList<QListWidgetItem*> pItemsFound = m_pUI->collectionList->findItems(sNewCollectionName, Qt::MatchRecursive);
            if (pItemsFound.size() > 0)
            {
                m_pUI->collectionList->setCurrentItem(pItemsFound[0]);
            }
            _refreshGridLayout(pNewCollection);
        }
        else
        {
            m_sCurrentCollection = "";
            m_sCurrentPlatform = m_Database.getPlatforms()[0]->getName();
            _refreshGridLayout(m_Database.getPlatform(m_sCurrentPlatform));
        }
    }
}

void MainWindow::on_collectionList_itemClicked(QListWidgetItem* a_pItem)
{
    // Get the selected collection
    QString sCollectionName = a_pItem->text();
    Collection* pCollection = m_Database.getCollection(sCollectionName);

    if (pCollection != NULL)
    {
        _refreshGridLayout(pCollection);
    }
}

void MainWindow::on_collectionSearch_returnPressed()
{
    QLineEdit* pLineEdit = dynamic_cast<QLineEdit*>(sender());
    if(pLineEdit != NULL)
    {
        // Search collection
        QString searchText = pLineEdit->text();

        // Search collection by name with lower case
        Collection* pCollection = NULL;
        foreach (Collection* pColl, m_Database.getCollections())
        {
            if (pColl->getName().toLower() == searchText.toLower())
            {
                pCollection = pColl;
                break;
            }
        }

        // Search collection which contains given text
        if (pCollection == NULL)
        {
            foreach (Collection* pColl, m_Database.getCollections())
            {
                if (pColl->getName().toLower().contains(searchText.toLower()))
                {
                    pCollection = pColl;
                    break;
                }
            }
        }

        // Set selected collection
        if (pCollection != NULL)
        {
            // Select collection in collection list view
            QList<QListWidgetItem*> pItemsFound = m_pUI->collectionList->findItems(pCollection->getName(), Qt::MatchRecursive);
            if (pItemsFound.size() > 0)
            {
                m_pUI->collectionList->setCurrentItem(pItemsFound[0]);
            }

            // Refresh games list
            _refreshGridLayout(pCollection);
        }
    }
}


//====================================================================================
// Slots MenuBar
//====================================================================================

void MainWindow::on_actionPreferences_triggered()
{
    // Accept action only if preferences window is not already displayed
    if (m_pPreferencesWindow->isHidden() == true)
    {
        // Update preference window content
        m_pPreferencesWindow->updateContent(m_Database);

        // Show preferences window (hidden when closed)
        m_pPreferencesWindow->show();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About ArcadeRoom"), "<p>"+tr("ArcadeRoom ")+QString(ARCADE_ROOM_VERSION)+"</p>\
                                                        <p>"+tr("Old games organizer with pretty user interface")+"</p>\
                                                        <p>"+tr("Copyright © 2015 - 2016 Fabrice Ballesio and Nicolas Daures")+"</p>\
                                                        <p>"+tr("All rights reserved.")+"</p>");
}

void MainWindow::on_actionQuit_triggered()
{
    exit(0);
}

void MainWindow::on_actionUndo_triggered()
{
    m_pUndoStack->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    m_pUndoStack->redo();
}

void MainWindow::on_languageChanged(QString a_sLanguage)
{
    _setLanguage(a_sLanguage);
}

void MainWindow::on_styleNameChanged(QString a_sStyleName)
{
    _setStyle(a_sStyleName);
}

void MainWindow::on_generalPreferencesChanged()
{
    // Write preferences json file
    QFile saveFile(m_sAppDirectory + "/preferences.json");

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open or create preferences file.");
        return;
    }

    QJsonObject jsonObject;
    m_pGeneralPreferences->write(jsonObject);
    QJsonDocument saveDoc(jsonObject);
    saveFile.write(saveDoc.toJson());
}

void MainWindow::on_actionConsoles_toggled(bool a_bIsChecked)
{
    m_pUI->tabWidget->setVisible(a_bIsChecked);
    m_pGeneralPreferences->setShowConsoles(a_bIsChecked);
    on_generalPreferencesChanged();
}

void MainWindow::on_actionTools_Bar_toggled(bool a_bIsChecked)
{
    m_pUI->GamesToolsBar->setVisible(a_bIsChecked);
    m_pGeneralPreferences->setShowToolsBar(a_bIsChecked);
    on_generalPreferencesChanged();
}

void MainWindow::on_actionStatus_Bar_toggled(bool a_bIsChecked)
{
    m_pUI->statusbar->setVisible(a_bIsChecked);
    m_pGeneralPreferences->setShowStatusBar(a_bIsChecked);
    on_generalPreferencesChanged();
}


//====================================================================================
// Slots ToolsBar
//====================================================================================

void MainWindow::on_buttonNoCover_clicked(bool checked)
{
    QString sIconPath = ":/icons/resources/icons/" + (m_pCurrentStyle != NULL ? m_pCurrentStyle->getIconStyle() : "white");
    m_pUI->buttonNoCover->setChecked(checked);
    if (checked == true)
    {
        // Button is not checked
        m_pUI->buttonNoCover->setToolTip(TOOLTIP_GAME_TAB_TOOLS_BAR_NO_COVER);
        m_pUI->buttonNoCover->setIcon(QIcon(sIconPath + "/no_cover.png"));
    }
    else
    {
        // Button is checked
        m_pUI->buttonNoCover->setToolTip(TOOLTIP_GAME_TAB_TOOLS_BAR_COVER);
        m_pUI->buttonNoCover->setIcon(QIcon(sIconPath + "/cover.png"));
    }

    // Stop the grid refresh
    m_iCurrentGameCount = -1;

    // Remove all games of the grid
    _clearGridLayout();

    // Write preferences
    m_pGeneralPreferences->setShowNoCovers(!checked);
    on_generalPreferencesChanged();

    // Restart the grid refresh
    m_iCurrentGameCount = 0;
    m_iCurrentGamePosition = 0;
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    m_iCurrentGameCount = -1;
}

void MainWindow::on_horizontalSlider_valueChanged(int a_iValue)
{
    m_fSizeFactor = a_iValue * 0.01;
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    // Refresh game layout
    _refreshGridLayout();

    // Update general preferences
    m_pGeneralPreferences->setCoverSizeFactor(m_fSizeFactor);
    on_generalPreferencesChanged();
}

void MainWindow::on_layoutTypeComboBox_currentIndexChanged(int a_iIndex)
{
    // Stop tick
    m_iCurrentGameCount = -1;

    // Delete old layout if needed
    _clearGridLayout();
    bool bChangeLayout = a_iIndex >= 3 || m_eGameLayoutType >= 3;
    if (bChangeLayout)
    {
        _deleteGameListWidget();
    }

    // Create new layout
    m_eGameLayoutType = (MainWindow::EGameLayoutType)a_iIndex;
    if (bChangeLayout)
    {
        _createGameListWidget();
    }

    // Update general preferences
    m_pGeneralPreferences->setLayoutType((uint)m_eGameLayoutType);
    on_generalPreferencesChanged();

    // Refresh game layout
    _refreshGridLayout();
}


//====================================================================================
// Slots Tick
//====================================================================================

void MainWindow::on_tickTriggered()
{
    // Add the next game to grid layout
    if (m_iCurrentGameCount != -1 && m_iCurrentGameCount < m_FilteredGames.size())
    {
        Game* pGame = m_FilteredGames[m_iCurrentGameCount];

        QString current = m_sAppDirectory + "/" + pGame->getPlatform()->getRomPath() + "/" + pGame->getRomRelativePath();

        // Get cover files
        QStringList coverFilter;
        coverFilter << "*.jpg";
        coverFilter << "*.png";
        QDir directory(current);
        QStringList coverFiles = directory.entryList(coverFilter);

        // No display if button is checked
        bool displayNoCoverRom = !m_pUI->buttonNoCover->isChecked();

        // Check if there is a cover for the rom
        bool romCoverExists = false;
        if (coverFiles.size() > 0)
        {
            // No cover
            romCoverExists = true;
        }

        // Get rom files
        QStringList romFilter = _getRomFilter(pGame->getPlatform());
        QStringList romFiles = directory.entryList(romFilter);

        // Add rom if rom file exists and there is a cover (or no cover roms have to be displayed)
        if ((romFiles.size() > 0) &&
            (romCoverExists == true || displayNoCoverRom == true || m_eGameLayoutType == List ))
        {
            // Add button and title to grid layout
            switch (m_eGameLayoutType)
            {
                case GridWith3Columns:
                case GridWith4Columns:
                case GridWith5Columns:
                {
                    QGroupBox* pGroupBox = _createGameGroupBox(pGame, romCoverExists, current, coverFiles, romFiles[0]);
                    int iColumnCount = m_eGameLayoutType + 3;
                    QGridLayout* pGridLayout = dynamic_cast<QGridLayout*>(m_pCurrentGameLayout);
                    pGridLayout->addWidget(pGroupBox, m_iCurrentGamePosition / iColumnCount, m_iCurrentGamePosition % iColumnCount, Qt::AlignHCenter);
                }
                break;
                case Horizontal:
                {
                    QGroupBox* pGroupBox = _createGameGroupBox(pGame, romCoverExists, current, coverFiles, romFiles[0]);
                    QHBoxLayout* pHBoxLayout = dynamic_cast<QHBoxLayout*>(m_pCurrentGameLayout);
                    pHBoxLayout->addWidget(pGroupBox);
                }
                break;
                case List:
                {
                    QTableWidget* pTableWidget = dynamic_cast<QTableWidget*>(m_pCurrentGameWidget);
                    if (pTableWidget->rowCount() != m_FilteredGames.size())
                    {
                        pTableWidget->setRowCount(m_FilteredGames.size());
                    }

                    QFileInfo fileInfo(romFiles[0]);
                    QTableWidgetItem* pItemName = new QTableWidgetItem(fileInfo.baseName());
                    pItemName->setFlags(Qt::ItemIsEnabled);
                    pItemName->setTextAlignment(Qt::AlignCenter);
                    pItemName->setStatusTip(romFiles[0]);
                    pTableWidget->setItem(m_iCurrentGamePosition, 0, pItemName);

                    QTableWidgetItem* pItemPlatform = new QTableWidgetItem(pGame->getPlatform()->getName());
                    pItemPlatform->setFlags(Qt::ItemIsEnabled);
                    pItemPlatform->setTextAlignment(Qt::AlignCenter);
                    pTableWidget->setItem(m_iCurrentGamePosition, 1, pItemPlatform);

                    QString starFilePath = QString(":/images/resources/images/rating_") + QString::number(pGame->getRating()) + QString(".png");
                    QTableWidgetItem* pItemNote = new QTableWidgetItem(QIcon(starFilePath), "");
                    pItemNote->setSizeHint(QSize(150, 25));
                    pItemNote->setFlags(Qt::ItemIsEnabled);
                    pTableWidget->setItem(m_iCurrentGamePosition, 2, pItemNote);
                }
                break;
                default:
                {
                    qWarning("Unkown game layout type");
                }
                break;
            }

            m_iCurrentGamePosition++;
        }

        m_iCurrentGameCount++;
    }
}


//====================================================================================
// Slots Database
//====================================================================================

void MainWindow::on_collectionCreated(Collection* a_pCollection)
{
    m_pUI->collectionList->addItem(a_pCollection->getName());
    connect(a_pCollection, SIGNAL(gameAdded(Game*)), this, SLOT(on_gameAddedToCollection(Game*)));
    connect(a_pCollection, SIGNAL(gameRemoved(Game*)), this, SLOT(on_gameRemovedFromCollection(Game*)));
}

void MainWindow::on_collectionDeleted(Collection* a_pCollection)
{
    QList<QListWidgetItem*> pItemsFound = m_pUI->collectionList->findItems(a_pCollection->getName(), Qt::MatchRecursive);
    if (pItemsFound.size() > 0)
    {
        delete pItemsFound[0];
    }
}

void MainWindow::on_gameAddedToCollection(Game*)
{
    Collection* pCollection = dynamic_cast<Collection*>(sender());
    if(pCollection != NULL && pCollection->getName() == m_sCurrentCollection)
    {
        _refreshGridLayout(pCollection);
    }
}

void MainWindow::on_gameRemovedFromCollection(Game*)
{
    Collection* pCollection = dynamic_cast<Collection*>(sender());
    if(pCollection != NULL && pCollection->getName() == m_sCurrentCollection)
    {
        _refreshGridLayout(pCollection);
    }
}

void MainWindow::on_comboBoxPlatformSorting_activated()
{
    // Modify tree view when a new platform sorting criteria is selected
    _fillTreeView();
}

//====================================================================================
// Private Operations
//====================================================================================

void MainWindow::_createGameListWidget()
{
    switch (m_eGameLayoutType)
    {
        case GridWith3Columns: // Grid with 3 columns
        case GridWith4Columns: // Grid with 4 columns
        case GridWith5Columns: // Grid with 5 columns
        case Horizontal: // Horizontal
        {
            // Create a scroll area
            QScrollArea* pScrollAreaGameWidget = new QScrollArea(m_pUI->tabGridContainer);
            pScrollAreaGameWidget->setAutoFillBackground(true);
            pScrollAreaGameWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            pScrollAreaGameWidget->setWidgetResizable(true);
            //m_pUI->gameListContainerLayout

            // Create a widget to display roms
            QWidget* pGameWidget = new QWidget(pScrollAreaGameWidget);
            pGameWidget->setGeometry(0, 0, 879, 539);

            // Set the widget to scroll area
            pScrollAreaGameWidget->setWidget(pGameWidget);
            m_pCurrentGameWidget = pScrollAreaGameWidget;

            if (m_eGameLayoutType == Horizontal)
            {
                // Set a new horizontal layout to widget
                m_pCurrentGameLayout = new QHBoxLayout(pGameWidget);
            }
            else
            {
                // Set a new grid layout to widget
                m_pCurrentGameLayout = new QGridLayout(pGameWidget);
            }
            pGameWidget->setLayout(m_pCurrentGameLayout);
        }
        break;
        case List: // List : work in progress
        {
            QTableWidget* pTableWidget = new QTableWidget(1, 3);
            pTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            pTableWidget->setIconSize(QSize(150, 25));
            connect(pTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(on_gameListItem_clicked(QTableWidgetItem*)));

            QStringList headers;
            headers << tr("Name") << tr("Platform") << tr("Note");
            pTableWidget->setHorizontalHeaderLabels(headers);
            pTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            pTableWidget->verticalHeader()->hide();

            m_pCurrentGameWidget = pTableWidget;
        }
        break;
        default:
        {
            qWarning("Unknown layout type.");
            return;
        }
        break;
    }

    // Add the scroll area to main ui
    m_pUI->gameListContainerLayout->addWidget(m_pCurrentGameWidget);

    // Update style if needed
    _updateWidgetsStyle();
}

void MainWindow::_deleteGameListWidget()
{
    delete m_pCurrentGameLayout;
    delete m_pCurrentGameWidget;

    m_pCurrentGameLayout = NULL;
    m_pCurrentGameWidget = NULL;
}

void MainWindow::_loadGeneralPreferences()
{
    m_pGeneralPreferences = new Preferences();

    // Open the general preferences file
    QFile loadFile(m_sAppDirectory + "/preferences.json");
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        m_pGeneralPreferences->setStyleName("dark");
        m_pGeneralPreferences->setShowNoCovers(true);

        m_pUI->actionConsoles->setChecked(true);
        m_pUI->tabWidget->setVisible(true);

        m_pUI->actionTools_Bar->setChecked(true);
        m_pUI->GamesToolsBar->setVisible(true);

        m_pUI->actionStatus_Bar->setChecked(true);
        m_pUI->statusbar->setVisible(true);

        _setStyle("dark");

        return;
    }

    // Read and deserialize
    QByteArray datas = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(datas));

    // Set general preferences
    QJsonObject jsonObject = loadDoc.object();
    m_pGeneralPreferences->read(jsonObject);

    bool bShowNoCovers = m_pGeneralPreferences->getShowNoCovers();
    m_pUI->buttonNoCover->setToolTip(bShowNoCovers ? TOOLTIP_GAME_TAB_TOOLS_BAR_COVER : TOOLTIP_GAME_TAB_TOOLS_BAR_NO_COVER);
    m_pUI->buttonNoCover->setChecked(!bShowNoCovers);

    bool bShowConsoles = m_pGeneralPreferences->getShowConsoles();
    m_pUI->actionConsoles->setChecked(bShowConsoles);
    m_pUI->tabWidget->setVisible(bShowConsoles);

    bool bShowToolsBar = m_pGeneralPreferences->getShowToolsBar();
    m_pUI->actionTools_Bar->setChecked(bShowToolsBar);
    m_pUI->GamesToolsBar->setVisible(bShowToolsBar);

    bool bShowStatusBar = m_pGeneralPreferences->getShowStatusBar();
    m_pUI->actionStatus_Bar->setChecked(bShowStatusBar);
    m_pUI->statusbar->setVisible(bShowStatusBar);

    float fCoverSizeFactor = m_pGeneralPreferences->getCoverSizeFactor();
    m_pUI->horizontalSlider->setValue((int)(fCoverSizeFactor * 100));

    // Apply preferences
    _setStyle(m_pGeneralPreferences->getStyleName());

    // Qt5 crash when set current index of combobox. Maybe the combo box is not ready.
    //int iLayoutType = m_pGeneralPreferences->getLayoutType();
    //m_pUI->layoutTypeComboBox->setCurrentIndex(iLayoutType);
}

void MainWindow::_clearGridLayout()
{
    if (m_pCurrentGameLayout != NULL)
    {
        QLayoutItem* item;
        while ((item = m_pCurrentGameLayout->takeAt(0)) != 0)
        {
            delete item->widget();
        }
    }
    else if (m_pCurrentGameWidget != NULL)
    {
        QTableWidget* pTableWidget = dynamic_cast<QTableWidget*>(m_pCurrentGameWidget);
        pTableWidget->setRowCount(0);
    }
}

void MainWindow::_refreshGridLayout(Collection* a_pCollection)
{
    m_iCurrentGameCount = -1;
    m_sCurrentCollection = a_pCollection->getName();
    m_sCurrentPlatform = "";

    _clearGridLayout();

    // Get the games and display them
    m_Games.clear();
    m_Games = a_pCollection->getGames();
    m_FilteredGames = m_Games;

    // Update status bar
    _refreshStatusBar();

    // Update layout type combo box here because combo box is not ready when load general preferences
    int iLayoutType = m_pGeneralPreferences->getLayoutType();
    if (m_pUI->layoutTypeComboBox->currentIndex() != iLayoutType)
    {
        m_pUI->layoutTypeComboBox->setCurrentIndex(iLayoutType);
    }

    // When current game count >= 0, the tick works
    m_iCurrentGameCount = 0;
    m_iCurrentGamePosition = 0;

    // Display games instead of overview
    m_pUI->gridContainer->setCurrentIndex(1);
}

void MainWindow::_refreshGridLayout(Platform* a_pPlatform)
{
    _loadGamesFromDirectory(a_pPlatform->getName());
}

void MainWindow::_refreshGridLayout()
{
    if (m_sCurrentPlatform != "")
    {
        Platform* pPlatform = m_Database.getPlatform(m_sCurrentPlatform);
        if (pPlatform != NULL)
        {
            _refreshGridLayout(pPlatform);
        }
    }
    else
    {
        Collection* pCollection = m_Database.getCollection(m_sCurrentCollection);
        if (pCollection != NULL)
        {
            _refreshGridLayout(pCollection);
        }
    }
}

void MainWindow::_loadGamesFromDirectory(const QString& a_sPlatformName)
{
    // Stop the grid refresh
    m_iCurrentGameCount = -1;
    m_sCurrentCollection = "";
    m_sCurrentPlatform = a_sPlatformName;

    // Clear the grid
    _clearGridLayout();

    Platform* pPlatform = m_Database.getPlatform(a_sPlatformName);
    if (pPlatform != NULL)
    {
        // Parse the roms directory
        QStringList filesAndDirectories;
        QDirIterator directories(m_sResourcePath + "/" + a_sPlatformName + "/Roms",
                                 QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while(directories.hasNext())
        {
            directories.next();
            filesAndDirectories << directories.filePath();
        }

        // Get the platform
        QString sPlatformName = pPlatform->getName();

        // Fill game list
        m_Games.clear();
        foreach (QString sGameDir, filesAndDirectories)
        {
            // Get rom files
            QStringList romFilter = _getRomFilter(pPlatform);
            QDir directory(sGameDir);
            QStringList romFiles = directory.entryList(romFilter);

            if (romFiles.size() > 0)
            {
                // Add the game to database
                Game* pGame = m_Database.getGame(sPlatformName, romFiles[0]);
                if (pGame != NULL)
                {
                    m_Games.append(pGame);
                }
            }
        }
        m_FilteredGames = m_Games;

        // Update status bar
        _refreshStatusBar();

        // Update layout type combo box here because combo box is not ready when load general preferences
        int iLayoutType = m_pGeneralPreferences->getLayoutType();
        if (m_pUI->layoutTypeComboBox->currentIndex() != iLayoutType)
        {
            m_pUI->layoutTypeComboBox->setCurrentIndex(iLayoutType);
        }

        // When current game count >= 0, the tick works
        m_iCurrentGameCount = 0;
        m_iCurrentGamePosition = 0;
    }
}

void MainWindow::_parseGamesFromDirectory(const QString& a_sPlatformName)
{
    // Parse the roms directory
    QStringList filesAndDirectories;
    QDirIterator directories(m_sResourcePath + "/" + a_sPlatformName + "/Roms",
                             QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(directories.hasNext())
    {
        directories.next();
        filesAndDirectories << directories.filePath();
    }

    // Get the platform
    Platform* pPlatform = m_Database.getPlatform(a_sPlatformName);

    // Fill game list
    foreach (QString sGameDir, filesAndDirectories)
    {
        // Get rom files
        QStringList romFilter = _getRomFilter(pPlatform);
        QDir directory(sGameDir);
        QStringList romFiles = directory.entryList(romFilter);

        if (romFiles.size() > 0)
        {
            // Add the game to database
            Game* pGame = m_Database.createGame(romFiles[0], QString(directory.dirName()), pPlatform);
            if (pGame != NULL)
            {
                m_Games.append(pGame);
            }
        }
    }
}

void MainWindow::_applyButtonStyle(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform)
{
    // Default shadow effect
    QGraphicsDropShadowEffect* pShadowEffect = new QGraphicsDropShadowEffect();
    pShadowEffect->setBlurRadius(10);
    pShadowEffect->setOffset(4,4);
    pShadowEffect->setColor(QColor(0,0,0));
    button->setGraphicsEffect(pShadowEffect);
    _setIconSize(button, groupBox, a_pPlatform);
}

QStringList MainWindow::_getRomFilter(Platform* a_pPlatform)
{
    QStringList romFilter;
    QStringList romExtensions = a_pPlatform->getRomExtensions();
    for (int iExtensionIndex = 0; iExtensionIndex < romExtensions.size(); ++iExtensionIndex)
    {
        QString extension = romExtensions[iExtensionIndex];
        romFilter << "*." + extension;
    }

    return romFilter;
}

void MainWindow::_setIconSize(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform)
{
    QSize iconSize = QSize((int)(a_pPlatform->getIconWidth() * m_fSizeFactor), (int)(a_pPlatform->getIconHeight() * m_fSizeFactor));
    button->setIconSize(iconSize);
    button->setFixedSize(iconSize);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QSize groupSize = QSize(iconSize.width() + 20, iconSize.height() + 50);
    groupBox->setFixedSize(groupSize);
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void MainWindow::_runGame(Game* a_pGame)
{
    Platform* pPlatform = a_pGame->getPlatform();
    QString gamePath = pPlatform->getRomPath() + "/" + a_pGame->getRomRelativePath() + "/" + a_pGame->getName();

    // Check if absolute path
    QFileInfo fileInfo(gamePath);
    if (!fileInfo.isAbsolute())
    {
        gamePath = m_sAppDirectory + '/' + gamePath;
    }

    QStringList arguments;
    QStringList emulatorArguments = pPlatform->getEmulatorArguments();
    if (emulatorArguments.size() > 0)
    {
        for (int iArgumentIndex = 0; iArgumentIndex < emulatorArguments.size(); ++iArgumentIndex)
        {
            QString argument = emulatorArguments[iArgumentIndex];

            // If the last argument contains '=', concatenate with rom path
            if (iArgumentIndex == emulatorArguments.size() - 1)
            {
                if (argument[argument.size()-1] == '=')
                {
                    arguments << argument + gamePath;
                }
                else
                {
                    arguments << argument;
                    arguments << gamePath;
                }
            }
            else
            {
                arguments << argument;
            }
        }
    }
    else
    {
        arguments << gamePath;
    }

    QProcess *process = new QProcess(this);
    process->start(pPlatform->getEmulatorPath(), arguments);
}

void MainWindow::_createCollection(QString a_sName)
{
    QUndoCommand* pCreateCollectionCommand = new CreateCollectionCommand(&m_Database, a_sName);
    m_pUndoStack->push(pCreateCollectionCommand);
}

void MainWindow::_deleteCollection(QString a_sName)
{
    QUndoCommand* pDeleteCollectionCommand = new DeleteCollectionCommand(&m_Database, a_sName);
    m_pUndoStack->push(pDeleteCollectionCommand);
}

void MainWindow::_setLanguage(QString a_sLanguage)
{
    // Remove previous translator
    qApp-> removeTranslator(&m_Translator);

    // Translation only if no default language is choosen.
    if (a_sLanguage != DEFAULT_LANGUAGE)
    {
        m_Translator.load("locales/"+a_sLanguage);
        qApp->installTranslator(&m_Translator);
    }

    // Refresh platform sorting criteria (or build it if empty)
    _refreshComboBoxPlatformSorting();

    // Apply changes on main window ui
    m_pUI->retranslateUi(this);

    // Apply changes on preferences windows ui
    m_pPreferencesWindow->retranslate();
}

void MainWindow::_setStyle(QString a_sStyleName)
{
    // Load style file from disk if exists
    qWarning(m_sAppDirectory.toStdString().c_str());
    QFile loadFile(m_sAppDirectory + "/styles/" + a_sStyleName + ".json");
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open style file.");
        return;
    }

    // Deserialize style
    QByteArray datas = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(datas));
    QJsonObject jsonObject = loadDoc.object();

    if (m_pCurrentStyle == NULL)
    {
        m_pCurrentStyle = new Style(a_sStyleName);
    }
    m_pCurrentStyle->read(jsonObject);

    // Initialize palette
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QColor baseColor = m_pCurrentStyle->getBaseColor();
    QColor textColor = m_pCurrentStyle->getTextColor();
    QColor windowColor = m_pCurrentStyle->getWindowColor();
    QColor highlightColor = m_pCurrentStyle->getHighlightColor();

    // Set palette
    QPalette p;
    p = qApp->palette();
    p.setColor(QPalette::Base, baseColor);
    p.setColor(QPalette::Text, textColor);
    p.setColor(QPalette::Window, windowColor);
    p.setColor(QPalette::WindowText, textColor);
    p.setColor(QPalette::Highlight, highlightColor);
    p.setColor(QPalette::HighlightedText, textColor);
    p.setColor(QPalette::Button, baseColor);
    p.setColor(QPalette::ButtonText, textColor);
    qApp->setPalette(p);

    // Update style of icons
    _updateIconsStyle();

    // Update style of widgets
    _updateWidgetsStyle();

    // Set application style
    QFile File(":/styles/resources/styles/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());
    qApp->setStyleSheet(StyleSheet);
}

void MainWindow::_updateIconsStyle()
{
    QString sIconPath = ":/icons/resources/icons/" + (m_pCurrentStyle != NULL ? m_pCurrentStyle->getIconStyle() : "white");

    m_pUI->collectionAddButton->setIcon(QIcon(sIconPath + "/add.png"));
    m_pUI->collectionDeleteButton->setIcon(QIcon(sIconPath + "/delete.png"));

    m_pUI->buttonNoCover->setIcon(!m_pUI->buttonNoCover->isChecked() ? QIcon(sIconPath + "/cover.png") : QIcon(sIconPath + "/no_cover.png"));
    m_pUI->coverSizeIcon->setPixmap(QPixmap(sIconPath + "/covers_size.png"));

    m_pUI->layoutTypeComboBox->setItemIcon(0, QIcon(sIconPath + "/layout_grid3xn.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(1, QIcon(sIconPath + "/layout_grid4xn.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(2, QIcon(sIconPath + "/layout_grid5xn.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(3, QIcon(sIconPath + "/layout_horizontal.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(4, QIcon(sIconPath + "/layout_list.png"));
}

void MainWindow::_updateWidgetsStyle()
{
    // Set background
    if (m_pCurrentGameWidget != NULL && m_pCurrentStyle != NULL)
    {
        m_pCurrentGameWidget->setStyleSheet("background-image: url(" + m_pCurrentStyle->getBackground() + ");" +
                                            "background-attachment: fixed;");
    }
}

void MainWindow::_fillTreeView()
{
    // Search all values for the sorting criteria
    QSet<QString> criteriaValues;

    // Init max number of characters for a criteria
    int criteriaMaxCharLength = 0;

    // Get all platforms
    QList<Platform*> platforms = m_Database.getPlatforms();

    // Pointer to the method which give the criteria value for a platform
    const QString& (Platform::*ptr)()const;

    // Get the selected criteria to sort platforms in tree view
    QString criteria = m_pUI->comboBoxPlatformSorting->currentText();

    // Remove all elements of the tree
    m_pUI->treeWidget->clear();

    // Get method which give the sorting criteria for a platform
    if (criteria == qApp->translate("CRITERIA_PLATFORM",CRITERIA_CONSTRUCTOR))
    {
        // Sort by constructor
        ptr = &Platform::getConstructorName;

        // Keep maximum letters for this criteria
        criteriaMaxCharLength = MAX_CHAR_PER_CRITERIA;
    }
    else if (criteria == qApp->translate("CRITERIA_PLATFORM",CRITERIA_GENERATION))
    {
        // Sort by generation
        ptr = &Platform::getGeneration;

        // Keep maximum letters for this criteria
        criteriaMaxCharLength = MAX_CHAR_PER_CRITERIA;
    }
    else
    {
        // Default, sort by name
        ptr = &Platform::getName;

        // Keep only one letter for alphabetic sorting
        criteriaMaxCharLength = 1;
    }

    // For each platform, insert value of sorting criteria
    foreach (Platform* pPlatform, platforms)
    {
        // Get criteria
        QString criteria = (pPlatform->*ptr)();

        // Keep only specified number of characters
        criteria.truncate(criteriaMaxCharLength);

        // Insert value
        criteriaValues.insert(criteria);
    }

    // Get the list of criteria values
    QStringList criteriaValuesList = QStringList(criteriaValues.toList());
    criteriaValuesList.sort();

    // Create the tree view which display consoles sorting with selected criteria
    QStringList::iterator it = criteriaValuesList.begin();
    QStringList::iterator end = criteriaValuesList.end();
    for (; it != end; ++it)
    {
        QTreeWidgetItem* pItem = new QTreeWidgetItem(QStringList(*it));
        m_pUI->treeWidget->addTopLevelItem(pItem);

        foreach (Platform* pPlatform, platforms)
        {
            QString criteria = (pPlatform->*ptr)();
            criteria.truncate(criteriaMaxCharLength);
            ((QString)(*it)).truncate(criteriaMaxCharLength);
            if (criteria == *it)
            {
                QTreeWidgetItem* pSubItem = new QTreeWidgetItem(QStringList(pPlatform->getName()));
                pSubItem->setIcon(0, QIcon(m_sAppDirectory + "/" + pPlatform->getIconPath()));
                pItem->addChild(pSubItem);
            }
        }
    }
}

void MainWindow::_setCurrentTreeViewItem(const QString& a_sPlatformName)
{
    QList<QTreeWidgetItem*> pItemsFound = m_pUI->treeWidget->findItems(a_sPlatformName, Qt::MatchRecursive);
    if (pItemsFound.size() > 0)
    {
        m_pUI->treeWidget->setCurrentItem(pItemsFound[0]);
        m_sCurrentCollection = "";
        m_sCurrentPlatform = a_sPlatformName;
        _refreshPlatformPanel();
        _loadGamesFromDirectory(m_sCurrentPlatform);
    }
}

void MainWindow::_deleteGame(const QString& a_sPlatformName, const QString& a_sGameName)
{
    // Remove game from database
    Game* pGame = m_Database.getGame(a_sPlatformName, a_sGameName);
    if (pGame)
    {
        m_Database.deleteGame(pGame);
    }
}

void MainWindow::_refreshPlatformPanel()
{
    Platform* pPlatform = m_Database.getPlatform(m_sCurrentPlatform);
    if (pPlatform != NULL)
    {
        // Load title
        QLabel* pTitle = m_pPlatformWidget->findChild<QLabel*>("title");
        if (pTitle != NULL)
        {
            pTitle->setText(pPlatform->getName());

            // Set big font size for title
            QFont font = pTitle->font();
            font.setPointSize(18);
            font.setBold(true);
            pTitle->setFont(font);
        }

        // Load description
        QLabel* pDescription = m_pPlatformWidget->findChild<QLabel*>("description");
        if (pDescription != NULL)
        {
            pDescription->setText(pPlatform->getDescription());
        }

        // Load image
        QPixmap pixmap = QPixmap(m_sAppDirectory + "/" + pPlatform->getImagePath());
        QLabel* pImage = m_pPlatformWidget->findChild<QLabel*>("image");
        pImage->setPixmap(pixmap);

        // Load screenshots
        QStringList screenshotUrls = pPlatform->getScreenshotUrls();

        QString url1 = screenshotUrls.count() > 0 ? m_sAppDirectory + "/" + screenshotUrls[0] : "";
        QPixmap pixmapScr1 = QPixmap(url1);
        QLabel* pScreenshotScr1 = m_pPlatformWidget->findChild<QLabel*>("screenshot_1");
        pScreenshotScr1->setPixmap(pixmapScr1);

        QString url2 = screenshotUrls.count() > 1 ? m_sAppDirectory + "/" + screenshotUrls[1] : "";
        QPixmap pixmapScr2 = QPixmap(url2);
        QLabel* pScreenshotScr2 = m_pPlatformWidget->findChild<QLabel*>("screenshot_2");
        pScreenshotScr2->setPixmap(pixmapScr2);

        QString url3 = screenshotUrls.count() > 1 ? m_sAppDirectory + "/" + screenshotUrls[2] : "";
        QPixmap pixmapScr3 = QPixmap(url3);
        QLabel* pScreenshotScr3 = m_pPlatformWidget->findChild<QLabel*>("screenshot_3");
        pScreenshotScr3->setPixmap(pixmapScr3);

        // TODO : Qt5 crashes when update layout and style of embed youtube video
        // Load videos
        /*QWebView* pWebView = pPlatformWidget->findChild<QWebView*>("webView_1");
        if (pWebView != NULL)
        {
            pWebView->setHtml("<html><body><div><iframe width=\"269\" height=\"145\" src=\"" + pPlatform->getVideoUrls()[0] + "\" frameborder=\"0\"></iframe></div></body></html>");
        }
        pWebView = pPlatformWidget->findChild<QWebView*>("webView_2");
        if (pWebView != NULL)
        {
            pWebView->setHtml("<html><body><div><iframe width=\"269\" height=\"145\" src=\"" + pPlatform->getVideoUrls()[1] + "\" frameborder=\"0\"></iframe></div></body></html>");
        }
        pWebView = pPlatformWidget->findChild<QWebView*>("webView_3");
        if (pWebView != NULL)
        {
            pWebView->setHtml("<html><body><div><iframe width=\"269\" height=\"145\" src=\"" + pPlatform->getVideoUrls()[2] + "\" frameborder=\"0\"></iframe></div></body></html>");
        }*/
    }
}

void MainWindow::_refreshStatusBar()
{
    m_pUI->statusbar->showMessage(QString(tr("     Consoles: ")) + QString::number(m_Database.getPlatforms().size()) +
                                  QString(tr("     Displayed Games: ")) + QString::number(m_FilteredGames.size()) +
                                  QString(tr("     Total Games: ")) + QString::number(m_Database.getGamesCount()));
}

void MainWindow::_refreshComboBoxPlatformSorting()
{
    // Build combobox items
    QStringList platformsSortingList;
    platformsSortingList << qApp->translate("CRITERIA_PLATFORM",CRITERIA_CONSTRUCTOR) << qApp->translate("CRITERIA_PLATFORM",CRITERIA_GENERATION) << qApp->translate("CRITERIA_PLATFORM",CRITERIA_NAME);

    // Get index of current selected item
    int currentIndex = m_pUI->comboBoxPlatformSorting->currentIndex();

    // If empty combobox, the first item will be selected.
    if (currentIndex < 0)
    {
        currentIndex = 0;
    }

    // No emit signal during refresh.
    m_pUI->comboBoxPlatformSorting->blockSignals(true);

    // Remove all items.
    m_pUI->comboBoxPlatformSorting->clear();

    // Inserts items.
    m_pUI->comboBoxPlatformSorting->insertItems(0,platformsSortingList);

    // Select previously selected item.
    m_pUI->comboBoxPlatformSorting->setCurrentIndex(currentIndex);

    // Reactivate signal emission.
    m_pUI->comboBoxPlatformSorting->blockSignals(false);
}

QGroupBox* MainWindow::_createGameGroupBox(Game* a_pGame, bool a_bIsCoverExists, QString a_sCurrentPath, QStringList a_sCoverFiles, QString a_sRomFile)
{
    // Create the cover with light effect
    /*QPixmap* pixmap = new QPixmap(400, 550);
    pixmap->fill(Qt::transparent);
    QPainter* painter = new QPainter(pixmap);
    painter->setCompositionMode(QPainter::CompositionMode_Lighten);
    painter->drawPixmap(0, 0, 400, 550, QPixmap(current + "/" + coverFiles[0]));
    painter->drawPixmap(0, 0, 400, 550, QPixmap(":/images/resources/images/light.png"));
    painter->end();*/

    // Path of the default cover file
    QString coverFilePath = ":/images/resources/images/no_cover.png";

    // Set cover file path if a cover exists for the rom
    if (a_bIsCoverExists == true)
    {
        coverFilePath = a_sCurrentPath + "/" + a_sCoverFiles[0];
    }

    // Create a button (icon + text)
    QToolButton *button = new QToolButton();
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIcon(QIcon(coverFilePath));//QIcon(*pixmap)
    connect(button, SIGNAL(clicked()), this, SLOT(on_button_clicked()));
    button->setProperty("platform", QVariant(a_pGame->getPlatform()->getName()));
    button->setProperty("game", QVariant(a_pGame->getName()));

    // Create title
    QFileInfo fileInfo(a_sRomFile);
    QString sFileNameWithoutExtension = fileInfo.baseName();
    QLabel* pLabel = new QLabel(sFileNameWithoutExtension);

    // Create a box which contains button and title
    QGroupBox* pGroupBox = new QGroupBox();
    pGroupBox->setObjectName(a_sRomFile); // TODO : remove duplication of properties (button and group)
    pGroupBox->setProperty("platform", QVariant(a_pGame->getPlatform()->getName()));
    pGroupBox->setProperty("game", QVariant(a_pGame->getName()));
    QVBoxLayout* pVboxLayout = new QVBoxLayout();
    pVboxLayout->addWidget(button, 0, Qt::AlignHCenter);
    pVboxLayout->addWidget(pLabel, 0, Qt::AlignHCenter);
    pGroupBox->setLayout(pVboxLayout);
    _applyButtonStyle(button, pGroupBox, a_pGame->getPlatform());

    // Add context menu
    pGroupBox->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(pGroupBox, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_groupBox_customContextMenu(const QPoint &)));

    return pGroupBox;
}

void MainWindow::_loadMetadatas()
{
    // Open the metadatas file
    QFile loadFile(m_sAppDirectory + "/metadatas.json");
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    // Read and deserialize
    QByteArray datas = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(datas));

    // Set ratings
    QJsonArray gameArray = loadDoc.object()["games"].toArray();
    QJsonArray::ConstIterator it = gameArray.constBegin();
    QJsonArray::ConstIterator end = gameArray.constEnd();
    for (; it != end; ++it)
    {
        QJsonObject json = it->toObject();
        QString sPlatformName = json["platform_name"].toString();
        QString sGameName = json["game_name"].toString();
        quint16 uiGameRating = json["game_rating"].toInt();
        Game* pGame = m_Database.getGame(sPlatformName, sGameName);
        if (pGame != NULL)
        {
            pGame->setRating(uiGameRating);
        }
    }
}

void MainWindow::_saveMetadatas()
{
    // Save game metadatas
    QFile saveFile(m_sAppDirectory + "/metadatas.json");
    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open or create metadatas file.");
        return;
    }

    QJsonObject jsonObject;
    QJsonArray gameArray;
    QList<QMap<QString, Game*> > gamesMap = m_Database.getGames();
    QList<QMap<QString, Game*> >::iterator it = gamesMap.begin();
    QList<QMap<QString, Game*> >::iterator end = gamesMap.end();
    for (; it != end; ++it)
    {
        QList<Game*> gamesList = it->values();
        QList<Game*>::iterator itGame = gamesList.begin();
        QList<Game*>::iterator endGame = gamesList.end();
        for (; itGame != endGame; ++itGame)
        {
            Game* pGameToSave = *itGame;

            if (pGameToSave->getRating() != 3)
            {
                QJsonObject json;
                json["platform_name"] = pGameToSave->getPlatform()->getName();
                json["game_name"] = pGameToSave->getName();
                json["game_rating"] = pGameToSave->getRating();
                gameArray.append(json);
            }
        }
    }
    jsonObject["games"] = gameArray;

    QJsonDocument saveDoc(jsonObject);
    saveFile.write(saveDoc.toJson());
}

#ifdef _DEBUG
void MainWindow::_createUndoView()
{
    m_pUndoView = new QUndoView(m_pUndoStack);
    m_pUndoView->setWindowTitle(tr("Command List"));
    m_pUndoView->show();
    m_pUndoView->setAttribute(Qt::WA_QuitOnClose, false);
}
#endif
