#include "platformlistwidget.h"
#include "ui_platformlistwidget.h"
#include "preferenceservice.h"
#include "sources/services/selectionservice.h"


//====================================================================================
// Constructors
//====================================================================================

PlatformListWidget::PlatformListWidget(QWidget* a_pParent)
: QWidget(a_pParent),
  m_pUI(new Ui::PlatformListWidget)
{
    m_pUI->setupUi(this);

    // Hide header of platform tree view
    m_pUI->treeWidget->header()->close();

    connect(m_pUI->comboBoxPlatformSorting, SIGNAL(currentIndexChanged(int)),  this, SLOT(on_comboBoxPlatformSorting_activated(int)));
    connect(m_pUI->treeSearch, SIGNAL(returnPressed()), this, SLOT(on_treeSearch_returnPressed()));
}

PlatformListWidget::~PlatformListWidget()
{
    delete m_pUI;
}


//====================================================================================
// Accessors
//====================================================================================

QTreeWidgetItem* PlatformListWidget::getCurrentItem()
{
    return m_pUI->treeWidget->currentItem();
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


//====================================================================================
// Private Slots
//====================================================================================

void PlatformListWidget::on_treeWidget_currentItemChanged(QTreeWidgetItem* a_pCurrent, QTreeWidgetItem* a_pPrevious)
{
    SelectionService* pSelectionService = SelectionService::getInstance();

    // Set the new platform and refresh the rom list
    QTreeWidgetItem* pItem = a_pCurrent != NULL ? a_pCurrent : a_pPrevious;
    QString newPlatform = pItem->text(m_pUI->treeWidget->currentColumn());
    if (newPlatform != pSelectionService->getCurrentPlatform())
    {
        QList<Platform*> platforms = DatabaseService::getInstance()->getPlatforms();
        foreach (Platform* pPlatform, platforms)
        {
            if (pPlatform->getName() == newPlatform)
            {
                pSelectionService->setCurrentCollection("");
                pSelectionService->setCurrentPlatform(pPlatform->getName());
                break;
            }
        }
    }
}

void PlatformListWidget::on_treeSearch_returnPressed()
{
    QLineEdit* pLineEdit = dynamic_cast<QLineEdit*>(sender());
    if(pLineEdit != NULL)
    {
        QString searchText = pLineEdit->text();
        QList<Platform*> platforms = DatabaseService::getInstance()->getPlatforms();

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

void PlatformListWidget::on_comboBoxPlatformSorting_activated(int a_iIndex)
{
    // Modify tree view when a new platform sorting criteria is selected
    fillTreeView();

    // Update general preferences
    // Get the selected criteria to sort platforms in tree view
    uint indexCriteria = a_iIndex;
    PreferenceService::getInstance()->setPlatformSortingCriteria(indexCriteria);
}


//====================================================================================
// Private Operations
//====================================================================================

void PlatformListWidget::_setCurrentTreeViewItem(const QString& a_sPlatformName)
{
    QList<QTreeWidgetItem*> pItemsFound = m_pUI->treeWidget->findItems(a_sPlatformName, Qt::MatchRecursive);
    if (pItemsFound.size() > 0)
    {
        m_pUI->treeWidget->setCurrentItem(pItemsFound[0]);

        SelectionService* pSelectionService = SelectionService::getInstance();
        pSelectionService->setCurrentCollection("");
        pSelectionService->setCurrentPlatform(a_sPlatformName);
    }
}
