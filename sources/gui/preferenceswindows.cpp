#include <QFileDialog>
#include <QFileInfo>
#include <stdio.h>

#include "mainwindow.h"
#include "preferenceswindows.h"
#include "ui_preferenceswindows.h"


//====================================================================================
// Constructors
//====================================================================================

PreferencesWindows::PreferencesWindows(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesWindows)
{
    ui->setupUi(this);

    // Make preferences window modal
    this->setWindowModality(Qt::WindowModal);

    DatabaseService* pDatabaseService = DatabaseService::getInstance();
    connect(this, SIGNAL(emulatorPathChanged(QString, QString)), pDatabaseService, SLOT(on_emulatorPathChanged(QString, QString)));
    connect(this, SIGNAL(romsPathChanged(QString, QString)), pDatabaseService, SLOT(on_romsPathChanged(QString, QString)));

    // TODO mettre m_PlatformPreferencesMap et m_CurrentPreferences vides ?
}

PreferencesWindows::~PreferencesWindows()
{
    delete ui;
}


//====================================================================================
// Operations
//====================================================================================

void PreferencesWindows::retranslate()
{
    ui->retranslateUi(this);
}

void PreferencesWindows::updateContent()
{
    // General preferences
    _updateGeneral();

    // Platforms
    _updatePlatform();
}

void PreferencesWindows::on_pushButtonBrowseEmulatorPath_clicked()
{
    QString emulatorPath;

    // Open a dialog to choose emulator for the selected current platform.
    emulatorPath = QFileDialog::getOpenFileName(this, tr("Choose Emulator"),
                                                "/home");

    // Nothing to do if no text entered (cancel button in QFileDialog)
    if (emulatorPath == "") return;

    // Display selected path in matching text area
    ui->textEditEmulatorPath->setText(emulatorPath);

    // Save this emulator path (internal save, only in this class)
    m_PlatformPreferencesMap[ui->comboBoxPlatformsList->currentText()].emulatorPath = emulatorPath;

    // Set flag to save emulator path in description file of current selected platform
    m_PlatformPreferencesMap[ui->comboBoxPlatformsList->currentText()].emulatorPathToSave = true;
}

void PreferencesWindows::on_comboBoxPlatformsList_activated(const QString &arg1)
{
    // Display emulator path in matching text area
    ui->textEditEmulatorPath->setText(m_PlatformPreferencesMap[arg1].emulatorPath);

    // Display roms path in matching text area
    ui->textEditRomsPath->setText(m_PlatformPreferencesMap[arg1].romsPath);
}

void PreferencesWindows::on_buttonApply_clicked()
{    
    // For each platform
    QMap<QString,preferencesPlatform>::Iterator it  = m_PlatformPreferencesMap.begin();
    QMap<QString,preferencesPlatform>::Iterator end = m_PlatformPreferencesMap.end();
    for (; it != end; ++it)
    {
        // Save current emulator path if needed
        if (it.value().emulatorPathToSave == true)
        {
            // Reset flag
            it.value().emulatorPathToSave = false;

            // Get emulator path
            QString emulatorPath = m_PlatformPreferencesMap[it.key()].emulatorPath;

            // Emit signal to database to save emulator path change
            emit emulatorPathChanged(emulatorPath,it.key());
        }

        // Save current roms path if needed
        if (it.value().romsPathToSave == true)
        {
            // Reset flag
            it.value().romsPathToSave = false;

            // Get emulator path
            QString romsPath = m_PlatformPreferencesMap[it.key()].romsPath;

            // Emit signal to database to save roms path change
            emit romsPathChanged(romsPath,it.key());
        }
    }

    // General preferences
    bool bPreferencesChanged = false;
    PreferenceService* pPreferenceService = PreferenceService::getInstance();

    QString oldStyleName = pPreferenceService->getStyleName();
    QString newStyleName = m_CurrentPreferences.getStyleName();
    if (oldStyleName != newStyleName)
    {
        pPreferenceService->setStyleName(newStyleName);
        bPreferencesChanged = true;
    }

    bool oldShowNoCovers = pPreferenceService->getShowNoCovers();
    bool newShowNoCovers = m_CurrentPreferences.getShowNoCovers();
    if (oldShowNoCovers != newShowNoCovers)
    {
        pPreferenceService->setShowNoCovers(newShowNoCovers);
        bPreferencesChanged = true;
    }

    QString oldLanguage = pPreferenceService->getLanguage();
    QString newLanguage = m_CurrentPreferences.getLanguage();
    if (oldLanguage != newLanguage)
    {
        pPreferenceService->setLanguage(newLanguage);
        bPreferencesChanged = true;
    }

    if (bPreferencesChanged)
    {
        emit generalPreferencesChanged();
    }
}

void PreferencesWindows::on_buttonClose_clicked()
{
    // Close the window.
    this->close();
}

void PreferencesWindows::on_pushButtonBrowseRomsPath_clicked()
{
    QString romsPath;

    // Open a dialog to choose roms path for the selected current platform.
    romsPath = QFileDialog::getExistingDirectory(this, tr("Choose roms directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // Nothing to do if no text entered (cancel button in QFileDialog)
    if (romsPath == "") return;

    // Display selected path in matching text area
    ui->textEditRomsPath->setText(romsPath);

    // Save this roms path (internal save, only in this class)
    m_PlatformPreferencesMap[ui->comboBoxPlatformsList->currentText()].romsPath = romsPath;

    // Set flag to save roms path in description file of current selected platform
    m_PlatformPreferencesMap[ui->comboBoxPlatformsList->currentText()].romsPathToSave = true;
}

void PreferencesWindows::on_styleComboBox_currentIndexChanged(const QString& a_sStyleName)
{
    m_CurrentPreferences.setStyleName(a_sStyleName);
}

void PreferencesWindows::on_languageComboBox_currentIndexChanged(const QString& a_sLanguage)
{
    m_CurrentPreferences.setLanguage(a_sLanguage);
}

void PreferencesWindows::_loadStyles()
{
    // Get style description files
    QStringList styleFilter;
    styleFilter << "*.json";
    QDir directory(QApplication::applicationDirPath() + "/styles");
    QStringList styleFiles = directory.entryList(styleFilter);

    // Remove all previously added styles
    ui->styleComboBox->clear();

    // Add a combo box items for each style
    foreach (QString styleFilePath, styleFiles)
    {
        QString styleName = styleFilePath.replace(".json", "");
        ui->styleComboBox->addItem(styleName);
    }
    ui->styleComboBox->setCurrentText(PreferenceService::getInstance()->getStyleName());
}

void PreferencesWindows::_updateGeneral()
{
    // Update styles
    _loadStyles();

    // List available language files
    QStringList languageFileFilter;
    languageFileFilter << "*.qm";
    QDir directory(QApplication::applicationDirPath() + "/locales");
    QStringList languageFiles = directory.entryList(languageFileFilter);

    // Remove file extension
    languageFiles.replaceInStrings(QRegExp("\\.qm$"), "");

    // Set list of languages
    //   - Remove all previously added languages
    ui->languageComboBox->clear();
    //   - Default
    ui->languageComboBox->insertItem(0,DEFAULT_LANGUAGE);
    //   - Other languages
    ui->languageComboBox->insertItems(1,languageFiles);

    // Set language combobox to favorite language (if available).
    languageFiles << DEFAULT_LANGUAGE;
    QString preferredLanguage = PreferenceService::getInstance()->getLanguage();
    if (languageFiles.contains(preferredLanguage))
    {
        ui->languageComboBox->setCurrentText(preferredLanguage);
    }
    else
    {
        qWarning("[PreferencesWindows::_updateGeneral]Favorite language '%s' is not available. Default has been set",preferredLanguage.toStdString().data());
        ui->languageComboBox->setCurrentText(DEFAULT_LANGUAGE);
    }
}

void PreferencesWindows::_updatePlatform()
{
    QString          platformsNames;
    QStringList      platformsNamesList;
    QList<Platform*> platformsList;
    QString          firstPlatformName;

    platformsNames="";
    firstPlatformName="";

    platformsList = DatabaseService::getInstance()->getPlatforms();

    // Get name of each platform and stock platform.
    for (int iPlatformIndex = 0; iPlatformIndex < platformsList.size(); ++iPlatformIndex)
    {
        QString platformName = platformsList[iPlatformIndex]->getName();
        if (iPlatformIndex == 0)
        {
            platformsNames = platformName;
            firstPlatformName = platformName;
        }
        else
        {
            platformsNames = platformsNames+';'+platformName;
        }

        // Stock emulator path
        m_PlatformPreferencesMap[platformName].emulatorPath = platformsList[iPlatformIndex]->getEmulatorPath();

        // No emulator path to save
        m_PlatformPreferencesMap[platformName].emulatorPathToSave = false;

        // Stock roms path
        m_PlatformPreferencesMap[platformName].romsPath = platformsList[iPlatformIndex]->getRomPath();

        // No roms path to save
        m_PlatformPreferencesMap[platformName].romsPathToSave = false;
    }

    // Build QStringList of platform names
    platformsNamesList=platformsNames.split(';');

    // Set list of platform names
    ui->comboBoxPlatformsList->clear();
    ui->comboBoxPlatformsList->insertItems(0,platformsNamesList);

    // Display emulator path in matching text area
    ui->textEditEmulatorPath->setText(m_PlatformPreferencesMap[firstPlatformName].emulatorPath);

    // Display roms path in matching text area
    ui->textEditRomsPath->setText(m_PlatformPreferencesMap[firstPlatformName].romsPath);
}
