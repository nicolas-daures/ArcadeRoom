#ifndef GAMELISTWIDGET_H
#define GAMELISTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSlider>
#include <QComboBox>
#include <QLabel>

namespace Ui
{
    class GameListWidget;
}

class GameListWidget : public QWidget
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    explicit GameListWidget(QWidget* a_pParent = NULL);
    ~GameListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    QWidget*                getGamesToolBar();
    QPushButton*            getButtonNoCover();
    QVBoxLayout*            getGameListContainerLayout();
    QSlider*                getHorizontalSlider();
    QComboBox*              getLayoutTypeComboBox();
    QLabel*                 getCoverSizeIcon();


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::GameListWidget*     m_pUI;
};

#endif // GAMELISTWIDGET_H
