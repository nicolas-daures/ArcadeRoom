#include "styleservice.h"
#include <QApplication>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStyleFactory>
#include <QPalette>


StyleService* StyleService::m_pInstance = NULL;


//====================================================================================
// Constructors
//====================================================================================

StyleService::StyleService()
: m_pCurrentStyle(NULL)
{

}


//====================================================================================
// Accessors
//====================================================================================

StyleService* StyleService::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new StyleService();
    }
    return m_pInstance;
}

Style* StyleService::getCurrentStyle()
{
    return m_pCurrentStyle;
}

void StyleService::setCurrentStyle(QString a_sName)
{
    if (m_StyleMap.contains(a_sName))
    {
        m_pCurrentStyle = m_StyleMap[a_sName];
        _applyStyle();
        emit styleSelected(m_pCurrentStyle);
    }
}


//====================================================================================
// Operations
//====================================================================================

void StyleService::loadStyles()
{
    // Parse the style directory
    QDir directory(QApplication::applicationDirPath() + "/styles/");
    QStringList files = directory.entryList(QStringList("*.json"));

    QStringList::const_iterator it = files.begin();
    QStringList::const_iterator end = files.end();
    for(; it != end; ++it)
    {
        QString sStyleFile = *it;
        QString sStyleName = sStyleFile.left(sStyleFile.size()-5);

        // Load style file from disk if exists
        QFile loadFile(QApplication::applicationDirPath() + "/styles/" + sStyleFile);
        if (!loadFile.open(QIODevice::ReadOnly))
        {
            qWarning("Couldn't open style file.");
            continue;
        }

        // Deserialize style
        QByteArray datas = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(datas));
        QJsonObject jsonObject = loadDoc.object();

        Style* pStyle = new Style(sStyleName);
        pStyle->read(jsonObject);

        m_StyleMap[sStyleName] = pStyle;
    }
}


//====================================================================================
// Private Operations
//====================================================================================

void StyleService::_applyStyle()
{
    // Initialize palette
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QColor baseColor = m_pCurrentStyle->getBaseColor();
    QColor textColor = m_pCurrentStyle->getTextColor();
    QColor windowColor = m_pCurrentStyle->getWindowColor();
    QColor highlightColor = m_pCurrentStyle->getHighlightColor();

    // Set palette
    QPalette p;
    p = qApp->palette();
    p.setColor(QPalette::Base, baseColor);
    p.setColor(QPalette::Text, textColor);
    p.setColor(QPalette::Window, windowColor);
    p.setColor(QPalette::WindowText, textColor);
    p.setColor(QPalette::Highlight, highlightColor);
    p.setColor(QPalette::HighlightedText, textColor);
    p.setColor(QPalette::Button, baseColor);
    p.setColor(QPalette::ButtonText, textColor);
    qApp->setPalette(p);
}

