#ifndef TRANSLATIONMANAGER_HPP
#define TRANSLATIONMANAGER_HPP

#include <QtQml/qqml.h>

#include <QGuiApplication>
#include <QObject>
#include <QTranslator>

class QQmlApplicationEngine;

/**
 * @brief Application translation and localization manager
 *
 * This class manages UI language translations, loading .qm files
 * and triggering QML engine reloads when language changes.
 * Supports multiple languages with proper BCP47 language codes.
 */
class TranslationManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ CurrentLanguage WRITE
                   SetCurrentLanguage NOTIFY currentLanguageChanged)

  public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit TranslationManager(QObject* parent = nullptr);

    /**
     * @brief Set the QML engine for retranslation
     * @param engine Pointer to QQmlApplicationEngine
     */
    void SetEngine(QQmlApplicationEngine* engine);

    /**
     * @brief Get current UI language
     * @return Language code string (e.g., "en_US")
     */
    QString CurrentLanguage() const;

    /**
     * @brief Set UI language
     * @param language Language code to activate
     */
    void SetCurrentLanguage(const QString& language);

    /**
     * @brief Get list of available UI languages
     * @return QStringList of language codes
     */
    Q_INVOKABLE QStringList AvailableLanguages() const;

    /**
     * @brief Get display name for a language code
     * @param languageCode Language code (e.g., "en_US")
     * @return Human-readable language name
     */
    Q_INVOKABLE QString LanguageDisplayName(const QString& languageCode) const;

  signals:
    void currentLanguageChanged();

  private:
    /**
     * @brief Load translation file for given language
     * @param language Language code
     */
    void LoadLanguage(const QString& language);

    QString m_currentLanguage;
    QTranslator m_translator;
    QGuiApplication* m_app;
    QQmlEngine* m_engine;
};

#endif  // TRANSLATIONMANAGER_HPP
