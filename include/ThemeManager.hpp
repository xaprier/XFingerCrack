#ifndef THEMEMANAGER_HPP
#define THEMEMANAGER_HPP

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

/**
 * @brief Application theme manager
 *
 * This class manages application themes, including loading theme
 * definitions from JSON, switching between themes, and exposing
 * theme data to QML for UI styling.
 */
class ThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(
        QVariantMap currentTheme READ CurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(QVariantList availableThemes READ AvailableThemes NOTIFY
                   availableThemesChanged)
    Q_PROPERTY(
        QString currentThemeId READ CurrentThemeId NOTIFY currentThemeIdChanged)

  public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit ThemeManager(QObject* parent = nullptr);

    /**
     * @brief Load themes from a JSON file
     * @param filePath Path to themes.json file
     * @return true if successful, false otherwise
     */
    bool LoadThemes(const QString& filePath);

    /**
     * @brief Get current theme data
     * @return QVariantMap containing theme colors and properties
     */
    QVariantMap CurrentTheme() const { return m_currentTheme; }

    /**
     * @brief Get list of all available themes
     * @return QVariantList containing theme information
     */
    QVariantList AvailableThemes() const { return m_availableThemes; }

    /**
     * @brief Get current theme identifier
     * @return Theme ID string
     */
    QString CurrentThemeId() const { return m_currentThemeId; }

    /**
     * @brief Set the active theme
     * @param themeId Theme identifier to activate
     */
    Q_INVOKABLE void SetTheme(const QString& themeId);

    /**
     * @brief Get theme data by ID
     * @param themeId Theme identifier
     * @return QVariantMap containing theme data
     */
    Q_INVOKABLE QVariantMap GetTheme(const QString& themeId) const;

  signals:
    void currentThemeChanged();
    void availableThemesChanged();
    void currentThemeIdChanged();

  private:
    QVariantMap m_currentTheme;
    QVariantList m_availableThemes;
    QString m_currentThemeId;
};

#endif  // THEMEMANAGER_HPP
