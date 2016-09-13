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
#include <QWebEngineView>
#include <QDesktopServices>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QScrollArea>
#include <QJsonArray>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "preferenceswindows.h"
#include "style.h"
#include "sources/database/databaseservice.h"
#include "sources/database/platform.h"
#include "sources/commands/addgametocollectioncommand.h"
#include "sources/commands/removegamefromcollectioncommand.h"
#include "sources/services/gamesfilterservice.h"
#include "styleservice.h"
#ifdef WIN32
#include <Windows.h>
#include <qset.h>
#endif //WIN32


//====================================================================================
// Constructors
//====================================================================================

MainWindow::MainWindow(QWidget* a_pParent) :
    QMainWindow(a_pParent),
    m_pUI(new Ui::MainWindow),
    m_sAppDirectory(QApplication::applicationDirPath()),
    m_pPlatformWidget(NULL)
{
    // Create undo stack
    CommandService::getInstance(this);

    // Load platforms
    DatabaseService* pDatabaseService = DatabaseService::getInstance();
    pDatabaseService->loadPlatforms();

    // Load games
    foreach (Platform* pPlatform, pDatabaseService->getPlatforms())
    {
        _parseGamesFromDirectory(pPlatform);
    }

    // Load metadatas
    _loadMetadatas();

    // Initialize UI
    m_pUI->setupUi(this);

    // Create a platform list panel
    m_pPlatformListWidget = new PlatformListWidget();
    m_pUI->tabConsoles->layout()->addWidget(m_pPlatformListWidget);

    // Create a collection list panel
    m_pCollectionListWidget = new CollectionListWidget();
    m_pUI->tabCollections->layout()->addWidget(m_pCollectionListWidget);

    // Create a game list panel
    m_pGameListWidget = new GameListWidget();
    m_pUI->tabGridContainer->layout()->addWidget(m_pGameListWidget);

    // Load styles
    StyleService::getInstance()->loadStyles();

    // Load general preferences
    _loadGeneralPreferences();
    m_pGameListWidget->createGameListWidget(m_pUI->tabGridContainer);

    // Create preferences window
    m_pPreferencesWindow = new PreferencesWindows(this);

    // Signal when general preferences updated
    PreferenceService* pPreferenceService = PreferenceService::getInstance();
    connect(pPreferenceService, SIGNAL(styleNameChanged(QString)), this, SLOT(on_styleNameChanged(QString)));
    connect(pPreferenceService, SIGNAL(languageChanged(QString)),  this, SLOT(on_languageChanged(QString)));

    // Signal when tree widget manipulated
    SelectionService* pSelectionService = SelectionService::getInstance();
    connect(pSelectionService, SIGNAL(platformSelected(QString)), this, SLOT(on_platformSelected(QString)));
    connect(pSelectionService, SIGNAL(collectionSelected(QString)), this, SLOT(on_collectionSelected(QString)));
    connect(pDatabaseService, SIGNAL(collectionCreated(Collection*)), this, SLOT(on_collectionCreated(Collection*)));

    // Fill treeview
    m_pPlatformListWidget->fillTreeView();

    // Connections
    connect(m_pGameListWidget, SIGNAL(gameParameterChanged()), this, SLOT(on_gameParameterChanged()));

    // Create a platform panel
    QVBoxLayout* pPlatformLayout = new QVBoxLayout(m_pUI->tabConsole);
    m_pUI->tabConsole->setLayout(pPlatformLayout);
    m_pPlatformWidget = new PlatformWidget();
    pPlatformLayout->addWidget(m_pPlatformWidget);
    pSelectionService->setCurrentPlatform("Nes");
    _refreshPlatformPanel();

    // Create a simple grid layout to display roms
    GamesFilterService::getInstance()->loadGames(pSelectionService->getCurrentPlatform());
    _refreshPlatformGridLayout(pSelectionService->getCurrentPlatform());

    // Translate application if requiered
    _setLanguage(pPreferenceService->getLanguage());

    // Create a tick object to load rom covers asynchronously
    m_pTick = new Tick();
    connect(m_pTick, SIGNAL(ticked()), m_pGameListWidget, SLOT(on_tickTriggered()));
    m_pTick->start();

    // Load collection on disk
    pDatabaseService->loadCollections();
    foreach (Collection* pCollection, pDatabaseService->getCollections())
    {
        m_pCollectionListWidget->addItem(pCollection->getName());
        connect(pCollection, SIGNAL(gameRemoved(Game*)), this, SLOT(on_gameRemovedFromCollection(Game*)));
    }
}

MainWindow::~MainWindow()
{
    delete m_pUI;
}


//====================================================================================
// Slots
//====================================================================================

void MainWindow::on_platformSelected(QString a_sPlatform)
{
     GamesFilterService::getInstance()->loadGames(a_sPlatform);

    _refreshPlatformPanel();
    _refreshGridLayout();
}

void MainWindow::on_collectionSelected(QString a_sCollection)
{
    Collection* pCollection = DatabaseService::getInstance()->getCollection(a_sCollection);
    GamesFilterService::getInstance()->setGames(pCollection->getGames());

    _refreshGridLayout();
}

void MainWindow::on_tabWidget_currentChanged(int a_iIndex)
{
    switch (a_iIndex)
    {
        case 0: // TreeView
        {
            QTreeWidgetItem* pItem = m_pPlatformListWidget->getCurrentItem();
            if (pItem != NULL)
            {
                QString sPlatformName = pItem->text(0);
                Platform* pPlatform = DatabaseService::getInstance()->getPlatform(sPlatformName);
                if (pPlatform)
                {
                    _refreshPlatformGridLayout(pPlatform->getName());
                }
            }
        }
        break;
        case 1: // ListView
        {
            QListWidgetItem* pItem = m_pCollectionListWidget->getCurrentItem();
            if (pItem != NULL)
            {
                QString sCollectionName = pItem->text();
                Collection* pCollection = DatabaseService::getInstance()->getCollection(sCollectionName);
                if (pCollection)
                {
                    _refreshCollectionGridLayout(pCollection->getName());
                }
            }
        }
        break;
    default:
        break;
    }
}

void MainWindow::on_collectionCreated(Collection* a_pCollection)
{
    connect(a_pCollection, SIGNAL(gameRemoved(Game*)), this, SLOT(on_gameRemovedFromCollection(Game*)));
}

void MainWindow::on_gameRemovedFromCollection(Game*)
{
    _refreshGridLayout();
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
        m_pPreferencesWindow->updateContent();

        // Show preferences window (hidden when closed)
        m_pPreferencesWindow->show();
    }
}

void MainWindow::on_actionDocumentation_triggered()
{
    // Open documentation website in default browser
    QDesktopServices::openUrl(QUrl("https://github.com/Dingodino/ArcadeRoom/wiki"));
}

void MainWindow::on_actionWebsite_triggered()
{
    // Open ArcadeRom website in default browser
    QDesktopServices::openUrl(QUrl("http://www.arcaderoom.org/"));
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
    CommandService::getInstance()->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    CommandService::getInstance()->redo();
}

void MainWindow::on_languageChanged(QString a_sLanguage)
{
    _setLanguage(a_sLanguage);
}

void MainWindow::on_styleNameChanged(QString a_sStyleName)
{
    _setStyle(a_sStyleName);
}

void MainWindow::on_actionConsoles_toggled(bool a_bIsChecked)
{
    m_pUI->tabWidget->setVisible(a_bIsChecked);
    PreferenceService::getInstance()->setShowConsoles(a_bIsChecked);
}

void MainWindow::on_actionTools_Bar_toggled(bool a_bIsChecked)
{
    m_pGameListWidget->setToolBarVisibility(a_bIsChecked);
    PreferenceService::getInstance()->setShowToolsBar(a_bIsChecked);
}

void MainWindow::on_actionStatus_Bar_toggled(bool a_bIsChecked)
{
    m_pUI->statusbar->setVisible(a_bIsChecked);
    PreferenceService::getInstance()->setShowStatusBar(a_bIsChecked);
}


//====================================================================================
// Slots Database
//====================================================================================

void MainWindow::on_gameParameterChanged()
{
    _refreshGridLayout();
}


//====================================================================================
// Private Operations
//====================================================================================

void MainWindow::_loadGeneralPreferences()
{
    // Open the general preferences file
    QFile loadFile(m_sAppDirectory + "/preferences.json");
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        PreferenceService::getInstance()->setStyleName("dark");
        PreferenceService::getInstance()->setShowNoCovers(true);

        m_pUI->actionConsoles->setChecked(true);
        m_pUI->tabWidget->setVisible(true);

        m_pUI->actionTools_Bar->setChecked(true);
        m_pGameListWidget->setToolBarVisibility(true);

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
    PreferenceService::getInstance()->read(jsonObject);

    bool bShowNoCovers = PreferenceService::getInstance()->getShowNoCovers();
    m_pGameListWidget->setButtonNoCoverToolTip(bShowNoCovers ? TOOLTIP_GAME_TAB_TOOLS_BAR_COVER : TOOLTIP_GAME_TAB_TOOLS_BAR_NO_COVER);
    m_pGameListWidget->setButtonNoCoverChecked(!bShowNoCovers);

    bool bShowConsoles = PreferenceService::getInstance()->getShowConsoles();
    m_pUI->actionConsoles->setChecked(bShowConsoles);
    m_pUI->tabWidget->setVisible(bShowConsoles);

    bool bShowToolsBar = PreferenceService::getInstance()->getShowToolsBar();
    m_pUI->actionTools_Bar->setChecked(bShowToolsBar);
    m_pGameListWidget->setToolBarVisibility(bShowToolsBar);

    bool bShowStatusBar = PreferenceService::getInstance()->getShowStatusBar();
    m_pUI->actionStatus_Bar->setChecked(bShowStatusBar);
    m_pUI->statusbar->setVisible(bShowStatusBar);

    float fCoverSizeFactor = PreferenceService::getInstance()->getCoverSizeFactor();
    m_pGameListWidget->setHorizontalSliderValue((int)(fCoverSizeFactor * 100));

    // Apply preferences
    _setStyle(PreferenceService::getInstance()->getStyleName());

    // Platform sorting criteria is done at combobox filling in
    // _refreshComboBoxPlatformSorting method

    // Qt5 crash when set current index of combobox. Maybe the combo box is not ready.
    //int iLayoutType = PreferenceService::getInstance()->getLayoutType();
    //m_pGameListWidget->getLayoutTypeComboBox()->setCurrentIndex(iLayoutType);
}

void MainWindow::_parseGamesFromDirectory(Platform* a_pPlatform)
{
    // Parse the roms directory
    QStringList filesAndDirectories;
    QDirIterator directories(a_pPlatform->getRomPath(),
                             QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(directories.hasNext())
    {
        directories.next();
        filesAndDirectories << directories.filePath();
    }

    // Fill game list
    foreach (QString sGameDir, filesAndDirectories)
    {
        // Get rom files
        QStringList romFilter =  GamesFilterService::getInstance()->createRomFilter(a_pPlatform);
        QDir directory(sGameDir);
        QStringList romFiles = directory.entryList(romFilter);

        if (romFiles.size() > 0)
        {
            // Add the game to database
            DatabaseService::getInstance()->createGame(romFiles[0], QString(directory.dirName()), a_pPlatform);
        }
    }
}

void MainWindow::_setLanguage(QString a_sLanguage)
{
    // Remove previous translator
    qApp-> removeTranslator(&m_Translator);

    // Translation only if no default language is choosen.
    if (a_sLanguage != DEFAULT_LANGUAGE)
    {
        m_Translator.load(m_sAppDirectory + "/locales/" + a_sLanguage);
        qApp->installTranslator(&m_Translator);
    }

    // Refresh platform sorting criteria (or build it if empty)
    m_pPlatformListWidget->refreshComboBoxPlatformSorting();

    // Apply changes on main window ui
    m_pUI->retranslateUi(this);

    // Apply changes on preferences windows ui
    m_pPreferencesWindow->retranslate();
}

void MainWindow::_setStyle(QString a_sStyleName)
{
    StyleService::getInstance()->setCurrentStyle(a_sStyleName);

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
    Style* pCurrentStyle = StyleService::getInstance()->getCurrentStyle();

    QString sIconPath = ":/icons/resources/icons/" + (pCurrentStyle != NULL ? pCurrentStyle->getIconStyle() : "white");

    m_pCollectionListWidget->setIcons(QIcon(sIconPath + "/add.png"), QIcon(sIconPath + "/delete.png"));

    m_pGameListWidget->updateIconsStyle(sIconPath);
}

void MainWindow::_updateWidgetsStyle()
{
    m_pGameListWidget->updateWidgetsStyle();
}

void MainWindow::_deleteGame(const QString& a_sPlatformName, const QString& a_sGameName)
{
    // Remove game from database
    Game* pGame = DatabaseService::getInstance()->getGame(a_sPlatformName, a_sGameName);
    if (pGame)
    {
        DatabaseService::getInstance()->deleteGame(pGame);
    }
}

void MainWindow::_refreshPlatformPanel()
{
    Platform* pPlatform = DatabaseService::getInstance()->getPlatform(SelectionService::getInstance()->getCurrentPlatform());
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
    m_pUI->statusbar->showMessage(QString(tr("     Consoles: ")) + QString::number(DatabaseService::getInstance()->getPlatforms().size()) +
                                  QString(tr("     Displayed Games: ")) + QString::number(m_pGameListWidget->getGames().size()) +
                                  QString(tr("     Total Games: ")) + QString::number(DatabaseService::getInstance()->getGamesCount()));
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
        Game* pGame = DatabaseService::getInstance()->getGame(sPlatformName, sGameName);
        if (pGame != NULL)
        {
            pGame->setRating(uiGameRating);
        }
    }
}

// TODO à regrouper avec _refreshPlatformGridLayout
void MainWindow::_refreshCollectionGridLayout(const QString& a_sCollection)
{
    // Stop the grid refresh
    m_pGameListWidget->stop();

    // Set collection
    SelectionService::getInstance()->setCurrentCollection(a_sCollection);

    // Remove all games in the game list widget
    m_pGameListWidget->clearGridLayout();

    // Set games in game list widget
    m_pGameListWidget->setGames(GamesFilterService::getInstance()->getGames());

    // Apply filter (if exists)
    GamesFilterService::getInstance()->applyFilter(GamesFilterService::getInstance()->getGames());

    // Set filtered games in game list widget
    m_pGameListWidget->setFilteredGames(GamesFilterService::getInstance()->getFilteredGames());

    // Update status bar
    _refreshStatusBar();

    // When current game count >= 0, the tick works to display games
    m_pGameListWidget->start();

    // Display games instead of overview
    m_pUI->gridContainer->setCurrentIndex(1);
}

void MainWindow::_refreshPlatformGridLayout(const QString& a_sPlatformName)
{  
    // Stop the grid refresh
    m_pGameListWidget->stop();

    // Set Platform
    SelectionService::getInstance()->setCurrentPlatform(a_sPlatformName);

    // Remove all games in the grid
    m_pGameListWidget->clearGridLayout();

    // Set games in game list widget
    m_pGameListWidget->setGames(GamesFilterService::getInstance()->getGames());

    // Apply filter (if exists)
    GamesFilterService::getInstance()->applyFilter(GamesFilterService::getInstance()->getGames());

    // Set filtered games in game list widget
    m_pGameListWidget->setFilteredGames(GamesFilterService::getInstance()->getFilteredGames());

    // Update status bar
    _refreshStatusBar();

    // Update layout type combo box here because combo box is not ready when load general preferences
    int iLayoutType = PreferenceService::getInstance()->getLayoutType();
    if (m_pGameListWidget->getCurrentLayoutType() != iLayoutType)
    {
        m_pGameListWidget->setCurrentLayoutType(iLayoutType);
    }

    // When current game count >= 0, the tick works to display games
    m_pGameListWidget->start();
}

void MainWindow::_refreshGridLayout()
{
    QString sPlatform = SelectionService::getInstance()->getCurrentPlatform();
    if (sPlatform != "")
    {
        Platform* pPlatform = DatabaseService::getInstance()->getPlatform(sPlatform);
        if (pPlatform != NULL)
        {
            _refreshPlatformGridLayout(pPlatform->getName());
        }
    }
    else
    {
        QString sCollection = SelectionService::getInstance()->getCurrentCollection();
        Collection* pCollection = DatabaseService::getInstance()->getCollection(sCollection);
        if (pCollection != NULL)
        {
            _refreshCollectionGridLayout(pCollection->getName());
        }
    }
}

