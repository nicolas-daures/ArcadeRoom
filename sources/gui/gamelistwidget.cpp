#include "gamelistwidget.h"
#include "ui_gamelistwidget.h"


//====================================================================================
// Constructors
//====================================================================================

GameListWidget::GameListWidget(QWidget* a_pParent)
: QWidget(a_pParent),
  m_pUI(new Ui::GameListWidget)
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

QWidget* GameListWidget::getGamesToolBar()
{
    return m_pUI->GamesToolsBar;
}

QPushButton* GameListWidget::getButtonNoCover()
{
    return m_pUI->buttonNoCover;
}

QVBoxLayout* GameListWidget::getGameListContainerLayout()
{
    return m_pUI->gameListContainerLayout;
}

QSlider* GameListWidget::getHorizontalSlider()
{
    return m_pUI->horizontalSlider;
}

QComboBox* GameListWidget::getLayoutTypeComboBox()
{
    return m_pUI->layoutTypeComboBox;
}

QLabel* GameListWidget::getCoverSizeIcon()
{
    return m_pUI->coverSizeIcon;
}
