#ifndef COLLECTIONLISTWIDGET_H
#define COLLECTIONLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QToolButton>

namespace Ui
{
    class CollectionListWidget;
}

class CollectionListWidget : public QWidget
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    explicit CollectionListWidget(QWidget *parent = 0);
    ~CollectionListWidget();


    //====================================================================================
    // Accessors
    //====================================================================================

    QListWidget*                getCollectionList();
    QToolButton*                getCollectionAddButton();
    QToolButton*                getCollectionDeleteButton();


private:

    //====================================================================================
    // Fields
    //====================================================================================

    Ui::CollectionListWidget*   m_pUI;
};

#endif // COLLECTIONLISTWIDGET_H
