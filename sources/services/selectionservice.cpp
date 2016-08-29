#include "selectionservice.h"
#include <QApplication>


SelectionService* SelectionService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

SelectionService::SelectionService()
{

}


//====================================================================================
// Accessors
//====================================================================================

SelectionService* SelectionService::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new SelectionService();
    }
    return m_pInstance;
}

QString SelectionService::getCurrentPlatform()
{
    return m_sCurrentPlatform;
}

QString SelectionService::getCurrentCollection()
{
    return m_sCurrentCollection;
}

void SelectionService::setCurrentPlatform(const QString& a_sPlatform)
{
    if (a_sPlatform != "" && m_sCurrentPlatform != a_sPlatform)
    {
        m_sCurrentPlatform = a_sPlatform;
        m_sCurrentCollection = "";
        emit platformSelected(a_sPlatform);
    }
}

void SelectionService::setCurrentCollection(const QString& a_sCollection)
{
    if (a_sCollection != "" && m_sCurrentCollection != a_sCollection)
    {
        m_sCurrentCollection = a_sCollection;
        m_sCurrentPlatform = "";
        emit collectionSelected(a_sCollection);
    }
}
