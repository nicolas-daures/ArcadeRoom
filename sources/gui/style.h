#ifndef STYLE_H
#define STYLE_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QJsonObject>

class Style : public QObject
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    Style(const QString& a_sName = "dark",
          const QColor& a_BaseColor = QColor(53,53,53),
          const QColor& a_TextColor = QColor(255,255,255),
          const QColor& a_WindowColor = QColor(30,30,30),
          const QColor& a_HighlightColor = QColor(142,45,197),
          const QString& a_sIconStyle = "black",
          const QString& a_sBackground = "");


    //====================================================================================
    // Accessors
    //====================================================================================

    const QString&              getName() const;
    void                        setName(const QString& a_sName);

    const QColor&               getBaseColor() const;
    void                        setBaseColor(const QColor& a_Color);

    const QColor&               getTextColor() const;
    void                        setTextColor(const QColor& a_Color);

    const QColor&               getWindowColor() const;
    void                        setWindowColor(const QColor& a_Color);

    const QColor&               getHighlightColor() const;
    void                        setHighlightColor(const QColor& a_Color);

    const QString&              getIconStyle() const;
    void                        setIconStyle(const QString& a_sIconStyle);

    const QString&              getBackground() const;
    void                        setBackground(const QString& a_sBackground);


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

    QString                     m_sName;
    QColor                      m_BaseColor;
    QColor                      m_TextColor;
    QColor                      m_WindowColor;
    QColor                      m_HighlightColor;
    QString                     m_sIconStyle;
    QString                     m_sBackground;
};

#endif // STYLE_H
