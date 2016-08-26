#ifndef SELECTIONSERVICE_H
#define SELECTIONSERVICE_H

#include <QObject>
#include <QString>

class SelectionService : public QObject
{
    Q_OBJECT


private:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a service to manage selection.
     ***********************************************************/
    SelectionService();


public:

    //====================================================================================
    // Accessors
    //====================================================================================

    static SelectionService*            getInstance();

    QString                             getCurrentPlatform();
    QString                             getCurrentCollection();

    void                                setCurrentPlatform(const QString& a_sPlatform);
    void                                setCurrentCollection(const QString& a_sCollection);


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when a platform is selected.
     * @param a_sPlatform : selected platform
     ***********************************************************/
    void                                platformSelected(QString a_sPlatform);

    /***********************************************************
     * @brief Called when a collection is selected.
     * @param a_sCollection : selected collection
     ***********************************************************/
    void                                collectionSelected(QString a_sCollection);


private:

    //====================================================================================
    // Fields
    //====================================================================================

    static SelectionService*            m_pInstance;

    // Current selection (ex: Nes, Super Nes, Genesis, Favoris, ...)
    QString                             m_sCurrentPlatform;

    // Current selection (ex: Mario Games Collection, ...)
    QString                             m_sCurrentCollection;
};

#endif // SELECTIONSERVICE_H
