#include "ThemeManager.hpp"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

ThemeManager::ThemeManager(QObject* parent) : QObject(parent), m_currentThemeId("dark") {}

bool ThemeManager::LoadThemes(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open themes file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid themes JSON format";
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray themesArray = root["themes"].toArray();

    m_availableThemes.clear();

    for (const QJsonValue& value : themesArray) {
        QJsonObject themeObj = value.toObject();
        QVariantMap theme;

        theme["name"] = themeObj["name"].toString();
        theme["id"] = themeObj["id"].toString();
        theme["background"] = themeObj["background"].toString();
        theme["surface"] = themeObj["surface"].toString();
        theme["text"] = themeObj["text"].toString();
        theme["textSecondary"] = themeObj["textSecondary"].toString();
        theme["correct"] = themeObj["correct"].toString();
        theme["incorrect"] = themeObj["incorrect"].toString();
        theme["current"] = themeObj["current"].toString();
        theme["accent"] = themeObj["accent"].toString();
        theme["border"] = themeObj["border"].toString();

        m_availableThemes.append(theme);

        // Set the first theme or the "dark" theme as current
        if (m_currentTheme.isEmpty() ||
            themeObj["id"].toString() == m_currentThemeId) {
            m_currentTheme = theme;
            m_currentThemeId = theme["id"].toString();
        }
    }

    emit availableThemesChanged();
    emit currentThemeChanged();
    emit currentThemeIdChanged();

    qDebug() << "Loaded" << m_availableThemes.size() << "themes";
    return !m_availableThemes.isEmpty();
}

void ThemeManager::SetTheme(const QString& themeId) {
    if (m_currentThemeId == themeId)
        return;

    for (const QVariant& variant : m_availableThemes) {
        QVariantMap theme = variant.toMap();
        if (theme["id"].toString() == themeId) {
            m_currentTheme = theme;
            m_currentThemeId = themeId;
            emit currentThemeChanged();
            emit currentThemeIdChanged();
            qDebug() << "Theme changed to:" << themeId;
            return;
        }
    }

    qWarning() << "Theme not found:" << themeId;
}

QVariantMap ThemeManager::GetTheme(const QString& themeId) const {
    for (const QVariant& variant : m_availableThemes) {
        QVariantMap theme = variant.toMap();
        if (theme["id"].toString() == themeId) {
            return theme;
        }
    }

    return QVariantMap();
}
