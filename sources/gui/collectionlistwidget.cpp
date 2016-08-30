#include "collectionlistwidget.h"
#include "ui_collectionlistwidget.h"
#include "sources/database/databaseservice.h"
#include "sources/database/collection.h"
#include "sources/commands/createcollectioncommand.h"
#include "sources/commands/deletecollectioncommand.h"
#include "sources/commands/commandservice.h"
#include "sources/services/selectionservice.h"

#include <QInputDialog>
#include <QMessageBox>


//====================================================================================
// Constructors
//====================================================================================

CollectionListWidget::CollectionListWidget(QWidget *parent)
: QWidget(parent),
  m_pUI(new Ui::CollectionListWidget)
{
    m_pUI->setupUi(this);

    DatabaseService* pDatabaseService = DatabaseService::getInstance();
    connect(pDatabaseService, SIGNAL(collectionCreated(Collection*)), this, SLOT(on_collectionCreated(Collection*)));
    connect(pDatabaseService, SIGNAL(collectionDeleted(Collection*)), this, SLOT(on_collectionDeleted(Collection*)));
}

CollectionListWidget::~CollectionListWidget()
{
    delete m_pUI;
}


//====================================================================================
// Accessors
//====================================================================================

QListWidgetItem* CollectionListWidget::getCurrentItem()
{
    return m_pUI->collectionList->currentItem();
}

void CollectionListWidget::setIcons(const QIcon& a_addIcon, const QIcon& a_deleteIcon)
{
    m_pUI->collectionAddButton->setIcon(a_addIcon);
    m_pUI->collectionDeleteButton->setIcon(a_deleteIcon);
}


//====================================================================================
// Operations
//====================================================================================

void CollectionListWidget::addItem(QString a_sCollectionName)
{
    m_pUI->collectionList->addItem(a_sCollectionName);
}


//====================================================================================
// Slots
//====================================================================================

void CollectionListWidget::on_collectionAddButton_clicked()
{
    bool bOk;

    QString sText = QInputDialog::getText(this, tr("Add a collection"),
                                         tr("Collection's name:"), QLineEdit::Normal,
                                         tr("Collection Name"), &bOk);

    if (bOk && !sText.isEmpty())
    {
        // Check if collection already exists
        Collection* pCollection = DatabaseService::getInstance()->getCollection(sText);
        if (pCollection == NULL)
        {
            CommandService::getInstance()->push(new CreateCollectionCommand(sText));
        }
        else
        {
            // Display an error dialog
            QMessageBox::warning(this, tr("Warning"), tr("This collection already exists !"), QMessageBox::Ok);
        }
    }
}

void CollectionListWidget::on_collectionDeleteButton_clicked()
{
    SelectionService* pSelectionService = SelectionService::getInstance();
    QString sCollection = pSelectionService->getCurrentCollection();
    if (sCollection != "")
    {
        int confirmDestroy = QMessageBox::question(this, tr("Delete collection"), tr("Do you really want to destroy this collection ?"), QMessageBox::Yes | QMessageBox::No);

        if (confirmDestroy == QMessageBox::Yes)
        {
            // Delete the collection
            CommandService::getInstance()->push(new DeleteCollectionCommand(sCollection));

            // Select another collection
            QList<Collection*> collections = DatabaseService::getInstance()->getCollections();
            if (collections.size() > 0)
            {
                Collection* pNewCollection = collections[0];
                QString sNewCollectionName = pNewCollection->getName();
                QList<QListWidgetItem*> pItemsFound = m_pUI->collectionList->findItems(sNewCollectionName, Qt::MatchRecursive);
                if (pItemsFound.size() > 0)
                {
                    m_pUI->collectionList->setCurrentItem(pItemsFound[0]);
                }
                pSelectionService->setCurrentCollection(pNewCollection->getName());
            }
            else if (DatabaseService::getInstance()->getPlatforms().size() > 0)
            {
                QString sPlatformName = DatabaseService::getInstance()->getPlatforms()[0]->getName();
                pSelectionService->setCurrentPlatform(sPlatformName);
            }
        }
    }
}

void CollectionListWidget::on_collectionList_itemClicked(QListWidgetItem* a_pItem)
{
    // Get the selected collection
    QString sCollectionName = a_pItem->text();
    Collection* pCollection = DatabaseService::getInstance()->getCollection(sCollectionName);

    if (pCollection != NULL)
    {
        SelectionService::getInstance()->setCurrentCollection(sCollectionName);
    }
}

void CollectionListWidget::on_collectionSearch_returnPressed()
{
    QLineEdit* pLineEdit = dynamic_cast<QLineEdit*>(sender());
    if(pLineEdit != NULL)
    {
        // Search collection
        QString searchText = pLineEdit->text();

        // Search collection by name with lower case
        Collection* pCollection = NULL;
        foreach (Collection* pColl, DatabaseService::getInstance()->getCollections())
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
            foreach (Collection* pColl, DatabaseService::getInstance()->getCollections())
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
            SelectionService::getInstance()->setCurrentCollection(pCollection->getName());
        }
    }
}

void CollectionListWidget::on_collectionCreated(Collection* a_pCollection)
{
    m_pUI->collectionList->addItem(a_pCollection->getName());
}

void CollectionListWidget::on_collectionDeleted(Collection* a_pCollection)
{
    QList<QListWidgetItem*> pItemsFound = m_pUI->collectionList->findItems(a_pCollection->getName(), Qt::MatchRecursive);
    if (pItemsFound.size() > 0)
    {
        delete pItemsFound[0];
    }
}

