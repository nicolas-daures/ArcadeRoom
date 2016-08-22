#include "collectionlistwidget.h"
#include "ui_collectionlistwidget.h"
#include "sources/database/databaseservice.h"
#include "sources/database/collection.h"
#include "sources/commands/createcollectioncommand.h"
#include "sources/commands/deletecollectioncommand.h"
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
}

CollectionListWidget::~CollectionListWidget()
{
    delete m_pUI;
}


//====================================================================================
// Accessors
//====================================================================================

QListWidget* CollectionListWidget::getCollectionList()
{
    return m_pUI->collectionList;
}

QToolButton* CollectionListWidget::getCollectionAddButton()
{
    return m_pUI->collectionAddButton;
}

QToolButton* CollectionListWidget::getCollectionDeleteButton()
{
    return m_pUI->collectionDeleteButton;
}

QString CollectionListWidget::getCurrentCollection() const
{
    return SelectionService::getInstance()->getCurrentCollection();
}

void CollectionListWidget::setCurrentCollection(QString a_sCollection)
{
    SelectionService::getInstance()->setCurrentCollection(a_sCollection);
}


//====================================================================================
// Operations
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
            _createCollection(sText);
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
    QString sCollection = SelectionService::getInstance()->getCurrentCollection();
    if (sCollection != "")
    {
        // Delete the collection
        _deleteCollection(sCollection);

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
            emit collectionSelected(pNewCollection);
        }
        else
        {
            SelectionService::getInstance()->setCurrentCollection("");
            emit collectionSelected(NULL);
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
        emit collectionSelected(pCollection);
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
            emit collectionSelected(pCollection);
        }
    }
}


//====================================================================================
// Private Operations
//====================================================================================

void CollectionListWidget::_createCollection(QString a_sName)
{
    QUndoCommand* pCreateCollectionCommand = new CreateCollectionCommand(a_sName);
    emit commandCreated(pCreateCollectionCommand);
}

void CollectionListWidget::_deleteCollection(QString a_sName)
{
    QUndoCommand* pDeleteCollectionCommand = new DeleteCollectionCommand(a_sName);
    emit commandCreated(pDeleteCollectionCommand);
}


