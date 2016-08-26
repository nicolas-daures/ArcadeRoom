#include "gamelistwidget.h"
#include "ui_gamelistwidget.h"
#include "sources/commands/commandservice.h"
#include "styleservice.h"
#include <QGroupBox>
#include <QUndoStack>
#include <QDir>
#include <QFileInfo>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QProcess>
#include <QMenu>
#include <QJsonArray>
#include <QJsonDocument>


//====================================================================================
// Constructors
//====================================================================================

GameListWidget::GameListWidget(QWidget* a_pParent)
: QWidget(a_pParent),
  m_pUI(new Ui::GameListWidget),
  m_iCurrentGameCount(-1),
  m_iCurrentGamePosition(0),
  m_pCurrentGameWidget(NULL),
  m_pCurrentGameLayout(NULL),
  m_fSizeFactor(1.0f),
  m_eGameLayoutType(GridWith3Columns)
{
    m_pUI->setupUi(this);
}

GameListWidget::~GameListWidget()
{
    delete m_pUI;
}


//====================================================================================
// Accessors
//====================================================================================

void GameListWidget::setToolBarVisibility(bool a_bVisibility)
{
    m_pUI->GamesToolsBar->setVisible(a_bVisibility);
}

void GameListWidget::setButtonNoCoverToolTip(QString a_sTooltip)
{
    m_pUI->buttonNoCover->setToolTip(a_sTooltip);
}

void GameListWidget::setButtonNoCoverChecked(bool a_bChecked)
{
    m_pUI->buttonNoCover->setChecked(a_bChecked);
}

void GameListWidget::setHorizontalSliderValue(int a_iSliderValue)
{
    m_pUI->horizontalSlider->setValue(a_iSliderValue);
}

int GameListWidget::getCurrentLayoutType()
{
    return m_pUI->layoutTypeComboBox->currentIndex();
}

void GameListWidget::setCurrentLayoutType(int a_iLayoutType)
{
    m_pUI->layoutTypeComboBox->setCurrentIndex(a_iLayoutType);
}

QList<Game*> GameListWidget::getGames()
{
    return m_FilteredGames;
}

void GameListWidget::setGames(QList<Game*> a_Games)
{
    m_Games = a_Games;
}

void GameListWidget::setFilteredGames(QList<Game*> a_Games)
{
    m_FilteredGames = a_Games;
}


//====================================================================================
// Slots
//====================================================================================

void GameListWidget::on_buttonNoCover_clicked(bool checked)
{
    Style* pStyle = StyleService::getInstance()->getCurrentStyle();

    QString sIconPath = ":/icons/resources/icons/" + (pStyle != NULL ? pStyle->getIconStyle() : "white");
    QPushButton* pButtonNoCover = m_pUI->buttonNoCover;
    pButtonNoCover->setChecked(checked);
    if (checked == true)
    {
        // Button is not checked
        pButtonNoCover->setToolTip(TOOLTIP_GAME_TAB_TOOLS_BAR_NO_COVER);
        pButtonNoCover->setIcon(QIcon(sIconPath + "/no_cover.png"));
    }
    else
    {
        // Button is checked
        pButtonNoCover->setToolTip(TOOLTIP_GAME_TAB_TOOLS_BAR_COVER);
        pButtonNoCover->setIcon(QIcon(sIconPath + "/cover.png"));
    }

    // Stop the grid refresh
    m_iCurrentGameCount = -1;

    // Remove all games of the grid
    clearGridLayout();

    // Write preferences
    PreferenceService* pPreferenceService = PreferenceService::getInstance();
    pPreferenceService->setShowNoCovers(!checked);
    pPreferenceService->save();

    // Restart the grid refresh
    m_iCurrentGameCount = 0;
    m_iCurrentGamePosition = 0;
}

void GameListWidget::on_horizontalSlider_sliderPressed()
{
    m_iCurrentGameCount = -1;
}

void GameListWidget::on_horizontalSlider_valueChanged(int a_iValue)
{
    m_fSizeFactor = a_iValue * 0.01;
}

void GameListWidget::on_horizontalSlider_sliderReleased()
{
    // Refresh game layout
    emit gameParameterChanged();

    //_refreshGridLayout();

    // Update general preferences
    PreferenceService* pPreferenceService = PreferenceService::getInstance();
    pPreferenceService->setCoverSizeFactor(m_fSizeFactor);
    pPreferenceService->save();
}

void GameListWidget::on_layoutTypeComboBox_currentIndexChanged(int a_iIndex)
{
    // Stop tick
    m_iCurrentGameCount = -1;

    // Delete old layout if needed
    clearGridLayout();
    bool bChangeLayout = a_iIndex >= 3 || m_eGameLayoutType >= 3;
    if (bChangeLayout)
    {
        _deleteGameListWidget();
    }

    // Create new layout
    m_eGameLayoutType = (GameListWidget::EGameLayoutType)a_iIndex;
    if (bChangeLayout)
    {
        createGameListWidget(NULL);
    }

    // Update general preferences
    PreferenceService* pPreferenceService = PreferenceService::getInstance();
    pPreferenceService->setLayoutType((uint)m_eGameLayoutType);
    pPreferenceService->save();

    // Refresh game layout
    emit gameParameterChanged();
}


void GameListWidget::on_buttonClicked()
{
    // Get the clicked rom
    QToolButton* button = dynamic_cast<QToolButton*>(sender());
    if( button != NULL )
    {
        QString sPlatformName = button->property("platform").toString();
        QString sGameName = button->property("game").toString();

        // Run emulator with rom
        Game* pGame = DatabaseService::getInstance()->getGame(sPlatformName, sGameName);
        _runGame(pGame);
    }
}

void GameListWidget::on_gridSearch_returnPressed()
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
        clearGridLayout();

        // When current game count >= 0, the tick works
        m_iCurrentGameCount = 0;
        m_iCurrentGamePosition = 0;
    }
}

void GameListWidget::on_gameListItemClicked(QTableWidgetItem* a_pTableItem)
{
    // Get the clicked game
    int iRowIndex = a_pTableItem->row();
    int iColumnIndex = a_pTableItem->column();
    QTableWidget* pTableWidget = dynamic_cast<QTableWidget*>(m_pCurrentGameWidget);
    QString sGameName = pTableWidget->item(iRowIndex, 0)->statusTip();
    QString sPlatformName = pTableWidget->item(iRowIndex, 1)->text();
    Game* pGame = DatabaseService::getInstance()->getGame(sPlatformName, sGameName);

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

void GameListWidget::on_tickTriggered()
{
    // Add the next game to grid layout
    if (m_iCurrentGameCount != -1 && m_iCurrentGameCount < m_FilteredGames.size())
    {
        Game* pGame = m_FilteredGames[m_iCurrentGameCount];

        QString current = pGame->getPlatform()->getRomPath() + "/" + pGame->getRomRelativePath();

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

void GameListWidget::on_groupBoxCustomContextMenu(const QPoint &point)
{
    QGroupBox* groupBox = dynamic_cast<QGroupBox*>(sender());
    if (groupBox != NULL)
    {
        QPoint globalPos = groupBox->mapToGlobal(point);

        // Get the selected game
        QString sPlatformName = groupBox->property("platform").toString();
        QString sGameName = groupBox->property("game").toString();
        QMap<QString, QUndoCommand*> commands = CommandService::getInstance()->getGameCommands(sPlatformName, sGameName);

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
                CommandService::getInstance()->push(commands[selectedItem->text()]);
            }
            else
            {
                // Run the game
                Game* pGame = DatabaseService::getInstance()->getGame(sPlatformName, sGameName);
                if (pGame != NULL)
                {
                    _runGame(pGame);
                }
            }
        }
    }
}


//====================================================================================
// Private Operations
//====================================================================================

void GameListWidget::start()
{
    // When current game count >= 0, the tick works
    m_iCurrentGameCount = 0;
    m_iCurrentGamePosition = 0;
}

void GameListWidget::stop()
{
    m_iCurrentGameCount = -1;
}

void GameListWidget::updateIconsStyle(QString a_sIconPath)
{
    m_pUI->buttonNoCover->setIcon(!m_pUI->buttonNoCover->isChecked() ? QIcon(a_sIconPath + "/cover.png") : QIcon(a_sIconPath + "/no_cover.png"));
    m_pUI->coverSizeIcon->setPixmap(QPixmap(a_sIconPath + "/covers_size.png"));

    m_pUI->layoutTypeComboBox->setItemIcon(0, QIcon(a_sIconPath + "/layout_grid3xn.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(1, QIcon(a_sIconPath + "/layout_grid4xn.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(2, QIcon(a_sIconPath + "/layout_grid5xn.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(3, QIcon(a_sIconPath + "/layout_horizontal.png"));
    m_pUI->layoutTypeComboBox->setItemIcon(4, QIcon(a_sIconPath + "/layout_list.png"));
}

void GameListWidget::clearGridLayout()
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

void GameListWidget::createGameListWidget(QWidget* a_pParent)
{
    if (a_pParent != NULL) m_pParentWidget = a_pParent;

    switch (m_eGameLayoutType)
    {
        case GridWith3Columns: // Grid with 3 columns
        case GridWith4Columns: // Grid with 4 columns
        case GridWith5Columns: // Grid with 5 columns
        case Horizontal: // Horizontal
        {
            // Create a scroll area
            QScrollArea* pScrollAreaGameWidget = new QScrollArea(m_pParentWidget);
            pScrollAreaGameWidget->setAutoFillBackground(true);
            pScrollAreaGameWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            pScrollAreaGameWidget->setWidgetResizable(true);

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
            connect(pTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(on_gameListItemClicked(QTableWidgetItem*)));

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
    updateWidgetsStyle();
}

void GameListWidget::updateWidgetsStyle()
{
    Style* pStyle = StyleService::getInstance()->getCurrentStyle();

    // Set background
    if (m_pCurrentGameWidget != NULL && pStyle != NULL)
    {
        m_pCurrentGameWidget->setStyleSheet("background-image: url(" + pStyle->getBackground() + ");" +
                                            "background-attachment: fixed;");
    }
}


//====================================================================================
// Private Operations
//====================================================================================

void GameListWidget::_deleteGameListWidget()
{
    delete m_pCurrentGameLayout;
    delete m_pCurrentGameWidget;

    m_pCurrentGameLayout = NULL;
    m_pCurrentGameWidget = NULL;
}

QGroupBox* GameListWidget::_createGameGroupBox(Game* a_pGame, bool a_bIsCoverExists, QString a_sCurrentPath, QStringList a_sCoverFiles, QString a_sRomFile)
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
    connect(button, SIGNAL(clicked()), this, SLOT(on_buttonClicked()));
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
    connect(pGroupBox, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_groupBoxCustomContextMenu(const QPoint &)));

    return pGroupBox;
}

void GameListWidget::_applyButtonStyle(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform)
{
    // Default shadow effect
    QGraphicsDropShadowEffect* pShadowEffect = new QGraphicsDropShadowEffect();
    pShadowEffect->setBlurRadius(10);
    pShadowEffect->setOffset(4,4);
    pShadowEffect->setColor(QColor(0,0,0));
    button->setGraphicsEffect(pShadowEffect);
    _setIconSize(button, groupBox, a_pPlatform);
}

void GameListWidget::_setIconSize(QToolButton* button, QGroupBox* groupBox, Platform* a_pPlatform)
{
    QSize iconSize = QSize((int)(a_pPlatform->getIconWidth() * m_fSizeFactor), (int)(a_pPlatform->getIconHeight() * m_fSizeFactor));
    button->setIconSize(iconSize);
    button->setFixedSize(iconSize);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QSize groupSize = QSize(iconSize.width() + 20, iconSize.height() + 50);
    groupBox->setFixedSize(groupSize);
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void GameListWidget::_runGame(Game* a_pGame)
{
    Platform* pPlatform = a_pGame->getPlatform();
    QString gamePath = pPlatform->getRomPath() + "/" + a_pGame->getRomRelativePath() + "/" + a_pGame->getName();
    QString emulatorPath = pPlatform->getEmulatorPath();

    // Check if absolute path
    QFileInfo gameFileInfo(gamePath);
    if (!gameFileInfo.isAbsolute())
    {
        gamePath = QApplication::applicationDirPath() + '/' + gamePath;
    }
    QFileInfo emulatorFileInfo(emulatorPath);
    if (!emulatorFileInfo.isAbsolute())
    {
        emulatorPath = QApplication::applicationDirPath() + '/' + emulatorPath;
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
                    qWarning((argument + gamePath).toStdString().c_str());
                    arguments << argument + gamePath;
                }
                else
                {
                    qWarning((argument).toStdString().c_str());
                    qWarning((gamePath).toStdString().c_str());
                    arguments << argument;
                    arguments << gamePath;
                }
            }
            else
            {
                qWarning(argument.toStdString().c_str());
                arguments << argument;
            }
        }
    }
    else
    {
        qWarning(gamePath.toStdString().c_str());
        arguments << gamePath;
    }

    qWarning(emulatorPath.toStdString().c_str());
    QProcess *process = new QProcess(this);
    process->start(emulatorPath, arguments);
}

void GameListWidget::_saveMetadatas()
{
    // Save game metadatas
    QFile saveFile(QApplication::applicationDirPath() + "/metadatas.json");
    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open or create metadatas file.");
        return;
    }

    QJsonObject jsonObject;
    QJsonArray gameArray;
    QList<QMap<QString, Game*> > gamesMap = DatabaseService::getInstance()->getGames();
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

QStringList GameListWidget::_getRomFilter(Platform* pPlatform)
{
    QStringList romFilter;
    QStringList romExtensions = pPlatform->getRomExtensions();
    for (int iExtensionIndex = 0; iExtensionIndex < romExtensions.size(); ++iExtensionIndex)
    {
        QString extension = romExtensions[iExtensionIndex];
        romFilter << "*." + extension;
    }
    return romFilter;
}
