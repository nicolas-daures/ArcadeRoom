#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QObject>
#include <QString>
#include <QJsonObject>

class Preferences : public QObject
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    Preferences();


    //====================================================================================
    // Accessors
    //====================================================================================

    const QString&              getLanguage() const;
    void                        setLanguage(const QString& a_sLanguage);

    const QString&              getStyleName() const;
    void                        setStyleName(const QString& a_sStyleName);

    bool                        getShowNoCovers() const;
    void                        setShowNoCovers(bool a_bShowNoCovers);

    bool                        getShowConsoles() const;
    void                        setShowConsoles(bool a_bShowConsoles);

    bool                        getShowToolsBar() const;
    void                        setShowToolsBar(bool a_bShowToolsBar);

    bool                        getShowStatusBar() const;
    void                        setShowStatusBar(bool a_bShowStatusBar);

    float                       getCoverSizeFactor() const;
    void                        setCoverSizeFactor(float a_fCoverSizeFactor);

    uint                        getLayoutType() const;
    void                        setLayoutType(uint a_uiLayoutType);

    uint                        getPlatformSortingCriteria() const;
    void                        setPlatformSortingCriteria(uint a_iIndexCriteria);


    //====================================================================================
    // Operations
    //====================================================================================

    /***********************************************************
     * @brief Deserialize the given JSON object.
     * @param a_json : JSON object to deserialize
     ***********************************************************/
    void                        read(const QJsonObject& a_json);

    /***********************************************************
     * @brief Serialize this in the given JSON object.
     * @param a_json : JSON object
     ***********************************************************/
    void                        write(QJsonObject& a_json) const;


private:

    //====================================================================================
    // Fields
    //====================================================================================

    QString                     m_sLanguage;
    QString                     m_sStyleName;
    bool                        m_bShowNoCovers;
    bool                        m_bShowConsoles;
    bool                        m_bShowToolsBar;
    bool                        m_bShowStatusBar;
    float                       m_fCoverSizeFactor;
    int                         m_iLayoutType;
    int                         m_iIndexCriteria;

};

#endif // PREFERENCES_H
