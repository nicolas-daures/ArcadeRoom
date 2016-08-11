#include "platformlistwidget.h"
#include "ui_platformlistwidget.h"


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
