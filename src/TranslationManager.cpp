#include "TranslationManager.hpp"

#include <QDebug>
#include <QDir>
#include <QQmlApplicationEngine>

TranslationManager::TranslationManager(QObject* parent)
    : QObject(parent),
      m_app(qobject_cast<QGuiApplication*>(QCoreApplication::instance())),
      m_engine(nullptr) {
    // Default language is English
    m_currentLanguage = "en_US";
    LoadLanguage(m_currentLanguage);
}

void TranslationManager::SetEngine(QQmlApplicationEngine* engine) {
    m_engine = engine;
}

QString TranslationManager::CurrentLanguage() const {
    return m_currentLanguage;
}

void TranslationManager::SetCurrentLanguage(const QString& language) {
    if (m_currentLanguage != language) {
        m_currentLanguage = language;
        LoadLanguage(language);
        emit currentLanguageChanged();
    }
}

QStringList TranslationManager::AvailableLanguages() const {
    return {"en_US", "tr_TR", "de_DE"};
}

QString
TranslationManager::LanguageDisplayName(const QString& languageCode) const {
    if (languageCode == "tr_TR") {
        return "Türkçe";
    } else if (languageCode == "en_US") {
        return "English";
    } else if (languageCode == "de_DE") {
        return "Deutsch";
    }
    return languageCode;
}

void TranslationManager::LoadLanguage(const QString& language) {
    if (!m_app) {
        qWarning() << "Application instance not found!";
        return;
    }

    // Remove previous translation
    m_app->removeTranslator(&m_translator);

    // Load new translation
    QString translationFile = QString(":/translations/translation_%1.qm").arg(language);

    if (m_translator.load(translationFile)) {
        m_app->installTranslator(&m_translator);
        qDebug() << "Loaded translation:" << translationFile;

        // Retranslate QML engine if available
        if (m_engine) {
            m_engine->retranslate();
            qDebug() << "Retranslated QML engine";
        }
    } else {
        qWarning() << "Failed to load translation:" << translationFile;
    }
}
