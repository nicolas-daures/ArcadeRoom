#include "platformlistwidget.h"
#include "ui_platformlistwidget.h"
#include "preferenceservice.h"


//====================================================================================
// Constructors
//====================================================================================

PlatformListWidget::PlatformListWidget(QWidget *parent)
: QWidget(parent),
  m_pUI(new Ui::PlatformListWidget)
{
    m_pUI->setupUi(this);
}

PlatformListWidget::~PlatformListWidget()
{
    delete m_pUI;
}


//====================================================================================
// Accessors
//====================================================================================

QTreeWidget* PlatformListWidget::getTreeWidget()
{
    return m_pUI->treeWidget;
}

QComboBox* PlatformListWidget::getComboBoxPlatformSorting()
{
    return m_pUI->comboBoxPlatformSorting;
}

QLineEdit* PlatformListWidget::getTreeSearch()
{
    return m_pUI->treeSearch;
}


//====================================================================================
// Operations
//====================================================================================

void PlatformListWidget::fillTreeView()
{
    // Search all values for the sorting criteria
    QSet<QString> criteriaValues;

    // Init max number of characters for a criteria
    int criteriaMaxCharLength = 0;

    // Get all platforms
    QList<Platform*> platforms = DatabaseService::getInstance()->getPlatforms();

    // Pointer to the method which give the criteria value for a platform
    const QString& (Platform::*ptr)()const;

    // Get the selected criteria to sort platforms in tree view
    QString criteria = m_pUI->comboBoxPlatformSorting->currentText();
    if (criteria == "") criteria = "Constructor";

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

    // Get application directory
    QString sAppDirectory = QApplication::applicationDirPath();

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
                pSubItem->setIcon(0, QIcon(sAppDirectory + "/" + pPlatform->getIconPath()));
                pItem->addChild(pSubItem);
            }
        }
    }
}

void PlatformListWidget::refreshComboBoxPlatformSorting()
{
    // Build combobox items
    QStringList platformsSortingList;
    platformsSortingList << qApp->translate("CRITERIA_PLATFORM",CRITERIA_CONSTRUCTOR) << qApp->translate("CRITERIA_PLATFORM",CRITERIA_GENERATION) << qApp->translate("CRITERIA_PLATFORM",CRITERIA_NAME);

    // Get index of current selected item
    QComboBox* pComboBox = m_pUI->comboBoxPlatformSorting;
    int currentIndex = pComboBox->currentIndex();

    // If empty combobox, the saved item in general preferences will be selected.
    if (currentIndex < 0)
    {
        currentIndex = PreferenceService::getInstance()->getPlatformSortingCriteria();
    }

    // No emit signal during refresh.
    pComboBox->blockSignals(true);
    // Remove all items.
    pComboBox->clear();

    // Inserts items.
    pComboBox->insertItems(0,platformsSortingList);

    // Select previously selected item.
    pComboBox->setCurrentIndex(currentIndex);

    // Reactivate signal emission.
    pComboBox->blockSignals(false);
}

