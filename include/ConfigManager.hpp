#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QObject>
#include <QSqlDatabase>
#include <QString>

/**
 * @brief Application configuration manager
 *
 * This class manages persistent application settings including
 * theme, test mode, duration, word count, language preferences,
 * and sound settings. Settings are automatically saved to SQLite database.
 */
class ConfigManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ Theme WRITE SetTheme NOTIFY themeChanged)
    Q_PROPERTY(int mode READ Mode WRITE SetMode NOTIFY modeChanged)
    Q_PROPERTY(
        int duration READ Duration WRITE SetDuration NOTIFY durationChanged)
    Q_PROPERTY(
        int wordCount READ WordCount WRITE SetWordCount NOTIFY wordCountChanged)
    Q_PROPERTY(
        QString language READ Language WRITE SetLanguage NOTIFY languageChanged)
    Q_PROPERTY(bool soundEnabled READ SoundEnabled WRITE SetSoundEnabled NOTIFY
                   soundEnabledChanged)
    Q_PROPERTY(bool soundOnClick READ SoundOnClick WRITE SetSoundOnClick NOTIFY
                   soundOnClickChanged)
    Q_PROPERTY(bool soundOnError READ SoundOnError WRITE SetSoundOnError NOTIFY
                   soundOnErrorChanged)
    Q_PROPERTY(QString clickSoundVariant READ ClickSoundVariant WRITE
                   SetClickSoundVariant NOTIFY clickSoundVariantChanged)
    Q_PROPERTY(qreal soundVolume READ SoundVolume WRITE SetSoundVolume NOTIFY
                   soundVolumeChanged)
    Q_PROPERTY(QString uiLanguage READ UiLanguage WRITE SetUiLanguage NOTIFY
                   uiLanguageChanged)

  public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit ConfigManager(QObject* parent = nullptr);

    // Getters
    QString Theme() const { return m_theme; }
    int Mode() const { return m_mode; }
    int Duration() const { return m_duration; }
    int WordCount() const { return m_wordCount; }
    QString Language() const { return m_language; }
    bool SoundEnabled() const { return m_soundEnabled; }
    bool SoundOnClick() const { return m_soundOnClick; }
    bool SoundOnError() const { return m_soundOnError; }
    QString ClickSoundVariant() const { return m_clickSoundVariant; }
    qreal SoundVolume() const { return m_soundVolume; }
    QString UiLanguage() const { return m_uiLanguage; }

    // Setters
    void SetTheme(const QString& theme);
    void SetMode(int mode);
    void SetDuration(int duration);
    void SetWordCount(int count);
    void SetLanguage(const QString& language);
    void SetSoundEnabled(bool enabled);
    void SetSoundOnClick(bool enabled);
    void SetSoundOnError(bool enabled);
    void SetClickSoundVariant(const QString& variant);
    void SetSoundVolume(qreal volume);
    void SetUiLanguage(const QString& language);

    /**
     * @brief Load settings from database
     */
    Q_INVOKABLE void LoadSettings();

    /**
     * @brief Save current settings to database
     */
    Q_INVOKABLE void SaveSettings();

    /**
     * @brief Get database file path
     * @return Absolute path to SQLite database file
     */
    Q_INVOKABLE QString GetDatabasePath() const;

  signals:
    void themeChanged();
    void modeChanged();
    void durationChanged();
    void wordCountChanged();
    void languageChanged();
    void soundEnabledChanged();
    void soundOnClickChanged();
    void soundOnErrorChanged();
    void clickSoundVariantChanged();
    void soundVolumeChanged();
    void uiLanguageChanged();

  private:
    /**
     * @brief Initialize SQLite database connection
     * @return true if successful, false otherwise
     */
    bool InitDatabase();

    /**
     * @brief Create database tables if they don't exist
     * @return true if successful, false otherwise
     */
    bool CreateTables();

    /**
     * @brief Save a specific setting to database
     * @param key Setting key
     * @param value Setting value
     */
    void SaveSetting(const QString& key, const QVariant& value);

    /**
     * @brief Load a setting from database
     * @param key Setting key
     * @param defaultValue Default value if key doesn't exist
     * @return Setting value
     */
    QVariant LoadSetting(const QString& key, const QVariant& defaultValue) const;

    QSqlDatabase m_database;
    QString m_theme;
    int m_mode;
    int m_duration;
    int m_wordCount;
    QString m_language;
    bool m_soundEnabled;
    bool m_soundOnClick;
    bool m_soundOnError;
    QString m_clickSoundVariant;
    qreal m_soundVolume;
    QString m_uiLanguage;
};

#endif  // CONFIGMANAGER_HPP
