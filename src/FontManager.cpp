#include "FontManager.hpp"

#include <QDebug>
#include <QFont>
#include <QGuiApplication>

FontManager::FontManager(QObject* parent)
    : QObject(parent), m_currentFont("JetBrains Mono") {
    LoadFonts();
}

void FontManager::LoadFonts() {
    qDebug() << "FontManager: Loading custom fonts...";

    // Load JetBrains Mono
    int id = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Regular.ttf");
    if (id != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty()) {
            m_availableFonts.append(families.first());
            qDebug() << "FontManager: Loaded JetBrains Mono:" << families.first();
        }
    } else {
        qWarning() << "FontManager: Failed to load JetBrains Mono";
    }

    // Load Cartograph CF
    id = QFontDatabase::addApplicationFont(":/fonts/CartographCF-Regular.ttf");
    if (id != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty()) {
            m_availableFonts.append(families.first());
            qDebug() << "FontManager: Loaded Cartograph CF:" << families.first();
        }
    } else {
        qWarning() << "FontManager: Failed to load Cartograph CF";
    }

    // Load MesloLGS NF
    id = QFontDatabase::addApplicationFont(":/fonts/MesloLGS-NF-Regular.ttf");
    if (id != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty()) {
            m_availableFonts.append(families.first());
            qDebug() << "FontManager: Loaded MesloLGS NF:" << families.first();
        }
    } else {
        qWarning() << "FontManager: Failed to load MesloLGS NF";
    }

    // Add system Monospace as an option
    m_availableFonts.append("Monospace");

    qDebug() << "FontManager: Available fonts:" << m_availableFonts;
}

void FontManager::SetFont(const QString& fontPreference) {
    QString newFont = GetFontFamily(fontPreference);
    if (m_currentFont != newFont) {
        m_currentFont = newFont;

        // Set application-wide default font
        QFont appFont(m_currentFont);
        QGuiApplication::setFont(appFont);

        qDebug() << "FontManager: Font changed to:" << m_currentFont;
        emit currentFontChanged();
    }
}

QString FontManager::GetFontFamily(const QString& fontPreference) const {
    // Map preference names to actual font families
    if (fontPreference == "JetBrains Mono") {
        // Return the actual loaded font family name
        for (const QString& font : m_availableFonts) {
            if (font.contains("JetBrains", Qt::CaseInsensitive)) {
                return font;
            }
        }
        return "JetBrains Mono";
    } else if (fontPreference == "Cartograph CF") {
        for (const QString& font : m_availableFonts) {
            if (font.contains("Cartograph", Qt::CaseInsensitive)) {
                return font;
            }
        }
        return "Cartograph CF";
    } else if (fontPreference == "MesloLGS NF") {
        for (const QString& font : m_availableFonts) {
            if (font.contains("Meslo", Qt::CaseInsensitive)) {
                return font;
            }
        }
        return "MesloLGS NF";
    } else if (fontPreference == "Monospace") {
        return "Monospace";
    }

    // Default to JetBrains Mono
    for (const QString& font : m_availableFonts) {
        if (font.contains("JetBrains", Qt::CaseInsensitive)) {
            return font;
        }
    }
    return "JetBrains Mono";
}
