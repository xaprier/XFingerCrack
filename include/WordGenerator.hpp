#ifndef WORDGENERATOR_HPP
#define WORDGENERATOR_HPP

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <random>

/**
 * @brief Word list data structure
 *
 * Contains language name, BCP47 code, and word list
 */
struct WordList {
    QString name;       ///< Language name (e.g., "English")
    QString bcp47;      ///< BCP47 language code (e.g., "en")
    QStringList words;  ///< List of words
};

/**
 * @brief Random word generator from language-specific word lists
 *
 * This class loads word lists from JSON files and generates random
 * word sequences for typing tests. Supports multiple languages
 * with proper shuffling and random selection.
 */
class WordGenerator : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit WordGenerator(QObject* parent = nullptr);

    /**
     * @brief Load all available wordlists from resources
     * @return true if successful, false otherwise
     */
    bool LoadAllWordlists();

    /**
     * @brief Load specific wordlist by language name
     * @param languageName Language name (e.g., "english")
     * @return true if successful, false otherwise
     */
    bool LoadWordlist(const QString& languageName);

    /**
     * @brief Get list of available languages
     * @return QStringList of language names
     */
    Q_INVOKABLE QStringList AvailableLanguages() const;

    /**
     * @brief Generate a random list of words
     * @param count Number of words to generate
     * @return QStringList of random words
     */
    Q_INVOKABLE QStringList GenerateWords(int count);

    /**
     * @brief Get the total number of available words in current language
     * @return Word count
     */
    Q_INVOKABLE int WordCount() const;

    /**
     * @brief Get current language name
     * @return Language name string
     */
    Q_INVOKABLE QString CurrentLanguage() const;

  private:
    /**
     * @brief Load wordlist from JSON file
     * @param filePath Path to JSON file
     * @return true if successful, false otherwise
     */
    bool LoadWordlistFromJson(const QString& filePath);

    QMap<QString, WordList> m_wordlists;
    QString m_currentLanguage;
    QStringList m_currentWords;
    std::mt19937 m_randomEngine;
};

#endif  // WORDGENERATOR_HPP
