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
#include "sources/database/platform.h"
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
    m_pPlatformWidget(NULL),
    m_pCurrentStyle(NULL)
{
    m_pDatabase = new Database();

    // Create undo stack
    CommandService::getInstance(m_pDatabase, this);

    // Load platforms
    m_pDatabase->loadPlatforms();

    // Load games
    foreach (Platform* pPlatform, m_pDatabase->getPlatforms())
    {
        _parseGamesFromDirectory(pPlatform);
    }

    // Load metadatas
    _loadMetadatas();

    // Initialize UI
    m_pUI->setupUi(this);

    // Create a platform list panel
    m_pPlatformListWidget = new PlatformListWidget(m_pDatabase);
    m_pUI->tabConsoles->layout()->addWidget(m_pPlatformListWidget);

    // Create a collection list panel
    m_pCollectionListWidget = new CollectionListWidget(m_pDatabase);
    m_pUI->tabCollections->layout()->addWidget(m_pCollectionListWidget);

    // Create a game list panel
    m_pGameListWidget = new GameListWidget(m_pDatabase);
    m_pUI->tabGridContainer->layout()->addWidget(m_pGameListWidget);

    // Load general preferences
    _loadGeneralPreferences();

    // Create preferences window
    m_pPreferencesWindow = new PreferencesWindows(this);

    // Signal when general preferences updated
    connect(PreferenceService::getInstance(), SIGNAL(styleNameChanged(QString)), this, SLOT(on_styleNameChanged(QString)));
    connect(PreferenceService::getInstance(), SIGNAL(languageChanged(QString)),  this, SLOT(on_languageChanged(QString)));
    connect(m_pPreferencesWindow, SIGNAL(emulatorPathChanged(QString, QString)), m_pDatabase, SLOT(on_emulatorPathChanged(QString, QString)));
    connect(m_pPreferencesWindow, SIGNAL(romsPathChanged(QString, QString)), m_pDatabase, SLOT(on_romsPathChanged(QString, QString)));
    connect(m_pPreferencesWindow, SIGNAL(generalPreferencesChanged()), this, SLOT(on_generalPreferencesChanged()));

    // Hide header of platform tree view
    m_pPlatformListWidget->getTreeWidget()->header()->close();

    // Signal when tree widget manipulated
    connect(m_pPlatformListWidget->getTreeWidget(), SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(on_treeWidget_clicked(QTreeWidgetItem*, QTreeWidgetItem*)));
    connect(m_pPlatformListWidget->getComboBoxPlatformSorting(), SIGNAL(currentIndexChanged(int)),  this, SLOT(on_comboBoxPlatformSorting_activated(int)));
    connect(m_pPlatformListWidget->getTreeSearch(), SIGNAL(returnPressed()), this, SLOT(on_treeSearch_returnPressed()));

    // Fill treeview
    m_pPlatformListWidget->fillTreeView();

    // Connections
    connect(m_pDatabase, SIGNAL(collectionCreated(Collection*)), this, SLOT(on_collectionCreated(Collection*)));
    connect(m_pDatabase, SIGNAL(collectionDeleted(Collection*)), this, SLOT(on_collectionDeleted(Collection*)));

    connect(m_pCollectionListWidget, SIGNAL(collectionSelected(Collection*)), this, SLOT(on_collectionSelected(Collection*)));
    connect(m_pCollectionListWidget, SIGNAL(commandCreated(QUndoCommand*)), this, SLOT(on_collectionCommandCreated(QUndoCommand*)));

    connect(m_pGameListWidget, SIGNAL(gameParameterChanged()), this, SLOT(on_gameParameterChanged()));

    // Create a platform panel
    QVBoxLayout* pPlatformLayout = new QVBoxLayout(m_pUI->tabConsole);
    m_pUI->tabConsole->setLayout(pPlatformLayout);
    m_pPlatformWidget = new PlatformWidget();
    pPlatformLayout->addWidget(m_pPlatformWidget);
    m_sCurrentPlatform = "Nes";
    _refreshPlatformPanel();

    // Create a simple grid layout to display roms
    m_pGameListWidget->_createGameListWidget(m_pUI->tabGridContainer);
    _loadGamesFromDirectory(m_sCurrentPlatform);

    // Translate application if requiered
    _setLanguage(PreferenceService::getInstance()->getLanguage());

    // Create a tick object to load rom covers asynchronously
    m_pTick = new Tick();
    connect(m_pTick, SIGNAL(ticked()), m_pGameListWidget, SLOT(on_tickTriggered()));
    m_pTick->start();

    // Load collection on disk
    m_pDatabase->loadCollections();
    foreach (Collection* pCollection, m_pDatabase->getCollections())
    {
        m_pCollectionListWidget->getCollectionList()->addItem(pCollection->getName());
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

void MainWindow::on_treeWidget_clicked(QTreeWidgetItem* a_pNext, QTreeWidgetItem* a_pPrevious)
{
    // Set the new platform and refresh the rom list
    QTreeWidgetItem* pItem = (a_pNext != NULL ? a_pNext : a_pPrevious);
    QString newPlatform = pItem->text(m_pPlatformListWidget->getTreeWidget()->currentColumn());
    if (newPlatform != m_sCurrentPlatform)
    {
        QList<Platform*> platforms = m_pDatabase->getPlatforms();
        foreach (Platform* pPlatform, platforms)
        {
            if (pPlatform->getName() == newPlatform)
            {
                m_pCollectionListWidget->setCurrentCollection("");
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
        QList<Platform*> platforms = m_pDatabase->getPlatforms();

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
            QTreeWidgetItem* pItem = m_pPlatformListWidget->getTreeWidget()->currentItem();
            if (pItem != NULL)
            {
                QString sPlatformName = pItem->text(0);
                Platform* pPlatform = m_pDatabase->getPlatform(sPlatformName);
                if (pPlatform)
                {
                    _refreshGridLayout(pPlatform);
                }
            }
        }
        break;
        case 1: // ListView
        {
            QListWidgetItem* pItem = m_pCollectionListWidget->getCollectionList()->currentItem();
            if (pItem != NULL)
            {
                QString sCollectionName = pItem->text();
                Collection* pCollection = m_pDatabase->getCollection(sCollectionName);
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
// Slots MenuBar
//====================================================================================

void MainWindow::on_actionPreferences_triggered()
{
    // Accept action only if preferences window is not already displayed
    if (m_pPreferencesWindow->isHidden() == true)
    {
        // Update preference window content
        m_pPreferencesWindow->updateContent(*m_pDatabase);

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

void MainWindow::on_generalPreferencesChanged()
{
    PreferenceService::getInstance()->save();
}

void MainWindow::on_actionConsoles_toggled(bool a_bIsChecked)
{
    m_pUI->tabWidget->setVisible(a_bIsChecked);
    PreferenceService::getInstance()->setShowConsoles(a_bIsChecked);
    on_generalPreferencesChanged();
}

void MainWindow::on_actionTools_Bar_toggled(bool a_bIsChecked)
{
    m_pGameListWidget->getGamesToolBar()->setVisible(a_bIsChecked);
    PreferenceService::getInstance()->setShowToolsBar(a_bIsChecked);
    on_generalPreferencesChanged();
}

void MainWindow::on_actionStatus_Bar_toggled(bool a_bIsChecked)
{
    m_pUI->statusbar->setVisible(a_bIsChecked);
    PreferenceService::getInstance()->setShowStatusBar(a_bIsChecked);
    on_generalPreferencesChanged();
}


//====================================================================================
// Slots Database
//====================================================================================

void MainWindow::on_collectionCreated(Collection* a_pCollection)
{
    m_pCollectionListWidget->getCollectionList()->addItem(a_pCollection->getName());
    connect(a_pCollection, SIGNAL(gameAdded(Game*)), this, SLOT(on_gameAddedToCollection(Game*)));
    connect(a_pCollection, SIGNAL(gameRemoved(Game*)), this, SLOT(on_gameRemovedFromCollection(Game*)));
}

void MainWindow::on_collectionDeleted(Collection* a_pCollection)
{
    QList<QListWidgetItem*> pItemsFound = m_pCollectionListWidget->getCollectionList()->findItems(a_pCollection->getName(), Qt::MatchRecursive);
    if (pItemsFound.size() > 0)
    {
        delete pItemsFound[0];
    }
}

void MainWindow::on_collectionSelected(Collection* a_pCollection)
{
    if (a_pCollection != NULL)
    {
        _refreshGridLayout(a_pCollection);
    }
    else
    {
        m_sCurrentPlatform = m_pDatabase->getPlatforms()[0]->getName();
        _refreshGridLayout(m_pDatabase->getPlatform(m_sCurrentPlatform));
    }
}

void MainWindow::on_collectionCommandCreated(QUndoCommand* a_pCommand)
{    
    CommandService::getInstance()->push(a_pCommand);
}

void MainWindow::on_gameAddedToCollection(Game*)
{
    Collection* pCollection = dynamic_cast<Collection*>(sender());
    if(pCollection != NULL && pCollection->getName() == m_pCollectionListWidget->getCurrentCollection())
    {
        _refreshGridLayout(pCollection);
    }
}

void MainWindow::on_gameRemovedFromCollection(Game*)
{
    Collection* pCollection = dynamic_cast<Collection*>(sender());
    if(pCollection != NULL && pCollection->getName() == m_pCollectionListWidget->getCurrentCollection())
    {
        _refreshGridLayout(pCollection);
    }
}

void MainWindow::on_comboBoxPlatformSorting_activated(int a_iIndex)
{
    // Modify tree view when a new platform sorting criteria is selected
    m_pPlatformListWidget->fillTreeView();

    // Update general preferences
    // Get the selected criteria to sort platforms in tree view
    uint indexCriteria = a_iIndex;
    PreferenceService::getInstance()->setPlatformSortingCriteria(indexCriteria);
    on_generalPreferencesChanged();
}

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
        m_pGameListWidget->getGamesToolBar()->setVisible(true);

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
    m_pGameListWidget->getButtonNoCover()->setToolTip(bShowNoCovers ? TOOLTIP_GAME_TAB_TOOLS_BAR_COVER : TOOLTIP_GAME_TAB_TOOLS_BAR_NO_COVER);
    m_pGameListWidget->getButtonNoCover()->setChecked(!bShowNoCovers);

    bool bShowConsoles = PreferenceService::getInstance()->getShowConsoles();
    m_pUI->actionConsoles->setChecked(bShowConsoles);
    m_pUI->tabWidget->setVisible(bShowConsoles);

    bool bShowToolsBar = PreferenceService::getInstance()->getShowToolsBar();
    m_pUI->actionTools_Bar->setChecked(bShowToolsBar);
    m_pGameListWidget->getGamesToolBar()->setVisible(bShowToolsBar);

    bool bShowStatusBar = PreferenceService::getInstance()->getShowStatusBar();
    m_pUI->actionStatus_Bar->setChecked(bShowStatusBar);
    m_pUI->statusbar->setVisible(bShowStatusBar);

    float fCoverSizeFactor = PreferenceService::getInstance()->getCoverSizeFactor();
    m_pGameListWidget->getHorizontalSlider()->setValue((int)(fCoverSizeFactor * 100));

    // Apply preferences
    _setStyle(PreferenceService::getInstance()->getStyleName());

    // Platform sorting criteria is done at combobox filling in
    // _refreshComboBoxPlatformSorting method

    // Qt5 crash when set current index of combobox. Maybe the combo box is not ready.
    //int iLayoutType = PreferenceService::getInstance()->getLayoutType();
    //m_pGameListWidget->getLayoutTypeComboBox()->setCurrentIndex(iLayoutType);
}

void MainWindow::_loadGamesFromDirectory(const QString& a_sPlatformName)
{
    // Stop the grid refresh
    m_pGameListWidget->stop();
    m_pCollectionListWidget->setCurrentCollection("");
    m_sCurrentPlatform = a_sPlatformName;

    // Clear the grid
    m_pGameListWidget->_clearGridLayout();

    Platform* pPlatform = m_pDatabase->getPlatform(a_sPlatformName);
    if (pPlatform != NULL)
    {
        // Parse the roms directory
        QStringList filesAndDirectories;
        QDirIterator directories(pPlatform->getRomPath(),
                                 QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while(directories.hasNext())
        {
            directories.next();
            filesAndDirectories << directories.filePath();
        }

        // Get the platform
        QString sPlatformName = pPlatform->getName();

        // Fill game list
        QList<Game*> games = QList<Game*>();
        foreach (QString sGameDir, filesAndDirectories)
        {
            // Get rom files
            QStringList romFilter = _getRomFilter(pPlatform);
            QDir directory(sGameDir);
            QStringList romFiles = directory.entryList(romFilter);

            if (romFiles.size() > 0)
            {
                // Add the game to database
                Game* pGame = m_pDatabase->getGame(sPlatformName, romFiles[0]);
                if (pGame != NULL)
                {
                    games.append(pGame);
                }
            }
        }
        m_pGameListWidget->setGames(games);
        m_pGameListWidget->setFilteredGames(games);

        // Update status bar
        _refreshStatusBar();

        // Update layout type combo box here because combo box is not ready when load general preferences
        int iLayoutType = PreferenceService::getInstance()->getLayoutType();
        if (m_pGameListWidget->getLayoutTypeComboBox()->currentIndex() != iLayoutType)
        {
            m_pGameListWidget->getLayoutTypeComboBox()->setCurrentIndex(iLayoutType);
        }

        m_pGameListWidget->start();
    }
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
        QStringList romFilter = _getRomFilter(a_pPlatform);
        QDir directory(sGameDir);
        QStringList romFiles = directory.entryList(romFilter);

        if (romFiles.size() > 0)
        {
            // Add the game to database
            m_pDatabase->createGame(romFiles[0], QString(directory.dirName()), a_pPlatform);
        }
    }
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
    m_pPlatformListWidget->refreshComboBoxPlatformSorting();

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

    m_pCollectionListWidget->getCollectionAddButton()->setIcon(QIcon(sIconPath + "/add.png"));
    m_pCollectionListWidget->getCollectionDeleteButton()->setIcon(QIcon(sIconPath + "/delete.png"));

    m_pGameListWidget->getButtonNoCover()->setIcon(!m_pGameListWidget->getButtonNoCover()->isChecked() ? QIcon(sIconPath + "/cover.png") : QIcon(sIconPath + "/no_cover.png"));
    m_pGameListWidget->getCoverSizeIcon()->setPixmap(QPixmap(sIconPath + "/covers_size.png"));

    m_pGameListWidget->getLayoutTypeComboBox()->setItemIcon(0, QIcon(sIconPath + "/layout_grid3xn.png"));
    m_pGameListWidget->getLayoutTypeComboBox()->setItemIcon(1, QIcon(sIconPath + "/layout_grid4xn.png"));
    m_pGameListWidget->getLayoutTypeComboBox()->setItemIcon(2, QIcon(sIconPath + "/layout_grid5xn.png"));
    m_pGameListWidget->getLayoutTypeComboBox()->setItemIcon(3, QIcon(sIconPath + "/layout_horizontal.png"));
    m_pGameListWidget->getLayoutTypeComboBox()->setItemIcon(4, QIcon(sIconPath + "/layout_list.png"));
}

void MainWindow::_updateWidgetsStyle()
{
    m_pGameListWidget->_updateWidgetsStyle(m_pCurrentStyle);
}

void MainWindow::_setCurrentTreeViewItem(const QString& a_sPlatformName)
{
    QList<QTreeWidgetItem*> pItemsFound = m_pPlatformListWidget->getTreeWidget()->findItems(a_sPlatformName, Qt::MatchRecursive);
    if (pItemsFound.size() > 0)
    {
        m_pPlatformListWidget->getTreeWidget()->setCurrentItem(pItemsFound[0]);
        m_pCollectionListWidget->setCurrentCollection("");
        m_sCurrentPlatform = a_sPlatformName;
        _refreshPlatformPanel();
        _loadGamesFromDirectory(m_sCurrentPlatform);
    }
}

void MainWindow::_deleteGame(const QString& a_sPlatformName, const QString& a_sGameName)
{
    // Remove game from database
    Game* pGame = m_pDatabase->getGame(a_sPlatformName, a_sGameName);
    if (pGame)
    {
        m_pDatabase->deleteGame(pGame);
    }
}

void MainWindow::_refreshPlatformPanel()
{
    Platform* pPlatform = m_pDatabase->getPlatform(m_sCurrentPlatform);
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
    m_pUI->statusbar->showMessage(QString(tr("     Consoles: ")) + QString::number(m_pDatabase->getPlatforms().size()) +
                                  QString(tr("     Displayed Games: ")) + QString::number(m_pGameListWidget->getGames().size()) +
                                  QString(tr("     Total Games: ")) + QString::number(m_pDatabase->getGamesCount()));
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
        Game* pGame = m_pDatabase->getGame(sPlatformName, sGameName);
        if (pGame != NULL)
        {
            pGame->setRating(uiGameRating);
        }
    }
}

void MainWindow::_refreshGridLayout(Collection* a_pCollection)
{
    m_pGameListWidget->stop();
    m_pCollectionListWidget->setCurrentCollection(a_pCollection->getName());
    m_sCurrentPlatform = "";

    m_pGameListWidget->_clearGridLayout();

    // Get the games and display them
    //m_Games.clear();
    m_pGameListWidget->setGames(a_pCollection->getGames());
    m_pGameListWidget->setFilteredGames(a_pCollection->getGames());

    // Update status bar
    _refreshStatusBar();

    // Update layout type combo box here because combo box is not ready when load general preferences
    int iLayoutType = PreferenceService::getInstance()->getLayoutType();
    // TODO
    /*if (m_pUI->layoutTypeComboBox->currentIndex() != iLayoutType)
    {
        m_pUI->layoutTypeComboBox->setCurrentIndex(iLayoutType);
    }*/

    // When current game count >= 0, the tick works
    m_pGameListWidget->start();

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
        Platform* pPlatform = m_pDatabase->getPlatform(m_sCurrentPlatform);
        if (pPlatform != NULL)
        {
            _refreshGridLayout(pPlatform);
        }
    }
    else
    {
        Collection* pCollection = m_pDatabase->getCollection(m_pCollectionListWidget->getCurrentCollection());
        if (pCollection != NULL)
        {
            _refreshGridLayout(pCollection);
        }
    }
}

