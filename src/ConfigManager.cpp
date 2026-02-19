#include "ConfigManager.hpp"

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

#include "Version.hpp"

ConfigManager::ConfigManager(QObject* parent)
    : QObject(parent),
      m_theme("Dark"),
      m_mode(0),  // Time based
      m_duration(30),
      m_wordCount(50),
      m_language("english"),
      m_soundEnabled(true),
      m_soundOnClick(true),
      m_soundOnError(true),
      m_clickSoundVariant("click4"),
      m_soundVolume(0.5),
      m_uiLanguage("tr_TR"),
      m_fontFamily("JetBrains Mono") {
    if (InitDatabase()) {
        LoadSettings();
    }
}

QString ConfigManager::GetDatabasePath() const {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString appName = QString(APP_NAME);

    return configPath + "/" + appName + ".db";
}

bool ConfigManager::InitDatabase() {
    QString dbPath = GetDatabasePath();
    qDebug() << "ConfigManager: Initializing database at" << dbPath;

    // Use unique connection name for each instance to avoid conflicts
    QString connectionName = QString("config_manager_%1").arg(reinterpret_cast<quintptr>(this));
    m_database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qWarning() << "ConfigManager: Failed to open database:"
                   << m_database.lastError().text();
        return false;
    }

    return CreateTables();
}

bool ConfigManager::CreateTables() {
    QSqlQuery query(m_database);

    // Create settings table with key-value pairs
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS settings (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL
        )
    )";

    if (!query.exec(createTableSQL)) {
        qWarning() << "ConfigManager: Failed to create settings table:"
                   << query.lastError().text();
        return false;
    }

    qDebug() << "ConfigManager: Settings table created/verified successfully";
    return true;
}

void ConfigManager::SaveSetting(const QString& key, const QVariant& value) {
    QSqlQuery query(m_database);

    // Use INSERT OR REPLACE for SQLite
    query.prepare(
        "INSERT OR REPLACE INTO settings (key, value) VALUES (:key, :value)");
    query.bindValue(":key", key);
    query.bindValue(":value", value.toString());

    if (!query.exec()) {
        qWarning() << "ConfigManager: Failed to save setting" << key << ":"
                   << query.lastError().text();
    }
}

QVariant ConfigManager::LoadSetting(const QString& key,
                                    const QVariant& defaultValue) const {
    QSqlQuery query(m_database);
    query.prepare("SELECT value FROM settings WHERE key = :key");
    query.bindValue(":key", key);

    if (query.exec() && query.next()) {
        return query.value(0);
    }

    return defaultValue;
}

void ConfigManager::SetTheme(const QString& theme) {
    if (m_theme != theme) {
        m_theme = theme;
        SaveSetting("theme", theme);
        emit themeChanged();
    }
}

void ConfigManager::SetMode(int mode) {
    if (m_mode != mode) {
        m_mode = mode;
        SaveSetting("mode", mode);
        emit modeChanged();
    }
}

void ConfigManager::SetDuration(int duration) {
    if (m_duration != duration) {
        m_duration = duration;
        SaveSetting("duration", duration);
        emit durationChanged();
    }
}

void ConfigManager::SetWordCount(int count) {
    if (m_wordCount != count) {
        m_wordCount = count;
        SaveSetting("wordCount", count);
        emit wordCountChanged();
    }
}

void ConfigManager::SetLanguage(const QString& language) {
    if (m_language != language) {
        qDebug() << "Language changing from" << m_language << "to" << language;
        m_language = language;
        SaveSetting("language", language);
        emit languageChanged();
    }
}

void ConfigManager::SetSoundEnabled(bool enabled) {
    if (m_soundEnabled != enabled) {
        m_soundEnabled = enabled;
        SaveSetting("soundEnabled", enabled);
        emit soundEnabledChanged();
    }
}

void ConfigManager::SetSoundOnClick(bool enabled) {
    if (m_soundOnClick != enabled) {
        m_soundOnClick = enabled;
        SaveSetting("soundOnClick", enabled);
        emit soundOnClickChanged();
    }
}

void ConfigManager::SetSoundOnError(bool enabled) {
    if (m_soundOnError != enabled) {
        m_soundOnError = enabled;
        SaveSetting("soundOnError", enabled);
        emit soundOnErrorChanged();
    }
}

void ConfigManager::SetClickSoundVariant(const QString& variant) {
    if (m_clickSoundVariant != variant) {
        m_clickSoundVariant = variant;
        SaveSetting("clickSoundVariant", variant);
        emit clickSoundVariantChanged();
    }
}

void ConfigManager::SetSoundVolume(qreal volume) {
    // Clamp volume between 0.0 and 1.0
    volume = qBound(0.0, volume, 1.0);
    if (qAbs(m_soundVolume - volume) > 0.01) {
        m_soundVolume = volume;
        SaveSetting("soundVolume", volume);
        emit soundVolumeChanged();
    }
}

void ConfigManager::SetUiLanguage(const QString& language) {
    if (m_uiLanguage != language) {
        m_uiLanguage = language;
        SaveSetting("uiLanguage", language);
        emit uiLanguageChanged();
    }
}

void ConfigManager::SetFontFamily(const QString& fontFamily) {
    if (m_fontFamily != fontFamily) {
        m_fontFamily = fontFamily;
        SaveSetting("fontFamily", fontFamily);
        emit fontFamilyChanged();
    }
}

void ConfigManager::LoadSettings() {
    m_theme = LoadSetting("theme", "Dark").toString();
    m_mode = LoadSetting("mode", 0).toInt();
    m_duration = LoadSetting("duration", 30).toInt();
    m_wordCount = LoadSetting("wordCount", 50).toInt();
    m_language = LoadSetting("language", "english").toString();
    m_soundEnabled = LoadSetting("soundEnabled", true).toBool();
    m_soundOnClick = LoadSetting("soundOnClick", true).toBool();
    m_soundOnError = LoadSetting("soundOnError", true).toBool();
    m_clickSoundVariant =
        LoadSetting("clickSoundVariant", "click4").toString();
    m_soundVolume = LoadSetting("soundVolume", 0.5).toReal();
    m_uiLanguage = LoadSetting("uiLanguage", "tr_TR").toString();
    m_fontFamily = LoadSetting("fontFamily", "JetBrains Mono").toString();

    qDebug() << "ConfigManager: Loaded settings - Language:" << m_language
             << "Theme:" << m_theme << "Mode:" << m_mode;

    // Emit all signals to update UI
    emit themeChanged();
    emit modeChanged();
    emit durationChanged();
    emit wordCountChanged();
    emit languageChanged();
    emit soundEnabledChanged();
    emit soundOnClickChanged();
    emit soundOnErrorChanged();
    emit clickSoundVariantChanged();
    emit soundVolumeChanged();
    emit uiLanguageChanged();
    emit fontFamilyChanged();
}

void ConfigManager::SaveSettings() {
    // Save all current settings to database
    SaveSetting("theme", m_theme);
    SaveSetting("mode", m_mode);
    SaveSetting("duration", m_duration);
    SaveSetting("wordCount", m_wordCount);
    SaveSetting("language", m_language);
    SaveSetting("soundEnabled", m_soundEnabled);
    SaveSetting("soundOnClick", m_soundOnClick);
    SaveSetting("soundOnError", m_soundOnError);
    SaveSetting("clickSoundVariant", m_clickSoundVariant);
    SaveSetting("soundVolume", m_soundVolume);
    SaveSetting("uiLanguage", m_uiLanguage);
    SaveSetting("fontFamily", m_fontFamily);

    qDebug() << "ConfigManager: All settings saved to database";
}
