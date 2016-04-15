#include "style.h"


//====================================================================================
// Constructors
//====================================================================================

Style::Style(const QString& a_sName,
      const QColor& a_BaseColor,
      const QColor& a_TextColor,
      const QColor& a_WindowColor,
      const QColor& a_HighlightColor,
      const QString& a_sIconStyle,
      const QString& a_sBackground)
: m_sName(a_sName),
  m_BaseColor(a_BaseColor),
  m_TextColor(a_TextColor),
  m_WindowColor(a_WindowColor),
  m_HighlightColor(a_HighlightColor),
  m_sIconStyle(a_sIconStyle),
  m_sBackground(a_sBackground)
{

}


//====================================================================================
// Accessors
//====================================================================================

const QString& Style::getName() const
{
    return m_sName;
}

void Style::setName(const QString& a_sName)
{
    m_sName = a_sName;
}

const QColor& Style::getBaseColor() const
{
    return m_BaseColor;
}

void Style::setBaseColor(const QColor& a_Color)
{
    m_BaseColor = a_Color;
}

const QColor& Style::getTextColor() const
{
    return m_TextColor;
}

void Style::setTextColor(const QColor& a_Color)
{
    m_TextColor = a_Color;
}

const QColor& Style::getWindowColor() const
{
    return m_WindowColor;
}

void Style::setWindowColor(const QColor& a_Color)
{
    m_WindowColor = a_Color;
}

const QColor& Style::getHighlightColor() const
{
    return m_HighlightColor;
}

void Style::setHighlightColor(const QColor& a_Color)
{
    m_HighlightColor = a_Color;
}

const QString& Style::getIconStyle() const
{
    return m_sIconStyle;
}

void Style::setIconStyle(const QString& a_sIconStyle)
{
    m_sIconStyle = a_sIconStyle;
}

const QString& Style::getBackground() const
{
    return m_sBackground;
}

void Style::setBackground(const QString& a_sBackground)
{
    m_sBackground = a_sBackground;
}


//====================================================================================
// Operations
//====================================================================================

void Style::read(const QJsonObject& a_json)
{
    // Style informations
    m_BaseColor = QColor(a_json["baseColor"].toString());
    m_TextColor = QColor(a_json["textColor"].toString());
    m_WindowColor = QColor(a_json["windowColor"].toString());
    m_HighlightColor = QColor(a_json["highlightColor"].toString());
    m_sIconStyle = a_json["iconStyle"].toString();
    m_sBackground = a_json["background"].toString();
}

void Style::write(QJsonObject& a_json) const
{
    // Style informations
    a_json["baseColor"] = m_BaseColor.name();
    a_json["textColor"] = m_TextColor.name();
    a_json["windowColor"] = m_WindowColor.name();
    a_json["highlightColor"] = m_HighlightColor.name();
    a_json["iconStyle"] = m_sIconStyle;
    a_json["background"] = m_sBackground;
}
