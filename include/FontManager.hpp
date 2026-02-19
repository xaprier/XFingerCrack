#ifndef FONTMANAGER_HPP
#define FONTMANAGER_HPP

#include <QFontDatabase>
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief Font management system
 *
 * Loads application fonts and provides access to font families
 */
class FontManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentFont READ CurrentFont NOTIFY currentFontChanged)
    Q_PROPERTY(QStringList availableFonts READ AvailableFonts CONSTANT)

  public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit FontManager(QObject* parent = nullptr);

    /**
     * @brief Get current font family
     * @return Current font family name
     */
    QString CurrentFont() const { return m_currentFont; }

    /**
     * @brief Get list of available custom fonts
     * @return List of font family names
     */
    QStringList AvailableFonts() const { return m_availableFonts; }

    /**
     * @brief Set current font by preference name
     * @param fontPreference Font preference (e.g., "JetBrains Mono", "Monospace")
     */
    Q_INVOKABLE void SetFont(const QString& fontPreference);

    /**
     * @brief Get actual font family name for a preference
     * @param fontPreference Font preference name
     * @return Actual font family name to use
     */
    Q_INVOKABLE QString GetFontFamily(const QString& fontPreference) const;

  signals:
    void currentFontChanged();

  private:
    /**
     * @brief Load all custom fonts from resources
     */
    void LoadFonts();

    QString m_currentFont;
    QStringList m_availableFonts;
    QFontDatabase m_fontDatabase;
};

#endif  // FONTMANAGER_HPP
