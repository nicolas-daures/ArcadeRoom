#include "platformwidget.h"
#include "ui_platformwidget.h"


//====================================================================================
// Constructors
//====================================================================================

PlatformWidget::PlatformWidget(QWidget *parent)
: QWidget(parent),
  m_pUI(new Ui::PlatformWidget)
{
    m_pUI->setupUi(this);
}

PlatformWidget::~PlatformWidget()
{
    delete m_pUI;
}
