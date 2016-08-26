#include "platformwidget.h"
#include "ui_platformwidget.h"


//====================================================================================
// Constructors
//====================================================================================

PlatformWidget::PlatformWidget(QWidget* a_pParent)
: QWidget(a_pParent),
  m_pUI(new Ui::PlatformWidget)
{
    m_pUI->setupUi(this);
}

PlatformWidget::~PlatformWidget()
{
    delete m_pUI;
}
