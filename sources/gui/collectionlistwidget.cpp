#include "collectionlistwidget.h"
#include "ui_collectionlistwidget.h"


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
