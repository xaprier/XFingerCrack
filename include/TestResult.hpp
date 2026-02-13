#ifndef TESTRESULT_HPP
#define TESTRESULT_HPP

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

/**
 * @brief Data point for WPM tracking over time
 *
 * Represents a single measurement of typing speed during a test
 */
struct WpmDataPoint {
    qint64 timestamp;  ///< Milliseconds since test start
    int wpm;           ///< Words per minute at this point
    double accuracy;   ///< Accuracy percentage at this point

    /**
     * @brief Convert to QVariantMap for QML
     * @return QVariantMap representation
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["timestamp"] = timestamp;
        map["wpm"] = wpm;
        map["accuracy"] = accuracy;
        return map;
    }
};

/**
 * @brief Character error information
 *
 * Records details about a typing error for analysis
 */
struct CharacterError {
    int position;      ///< Position in target text
    QChar expected;    ///< Expected character
    QChar actual;      ///< What user typed
    qint64 timestamp;  ///< When error occurred (ms since start)

    /**
     * @brief Convert to QVariantMap for QML
     * @return QVariantMap representation
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["position"] = position;
        map["expected"] = QString(expected);
        map["actual"] = QString(actual);
        map["timestamp"] = timestamp;
        return map;
    }
};

/**
 * @brief Character statistics for analysis
 *
 * Tracks typing accuracy for individual characters
 */
struct CharacterStats {
    QChar character;     ///< The character
    int totalTyped;      ///< Total times typed
    int correctTyped;    ///< Correct occurrences
    int incorrectTyped;  ///< Incorrect occurrences

    /**
     * @brief Calculate accuracy percentage
     * @return Accuracy as percentage
     */
    double accuracy() const {
        return totalTyped > 0 ? (double)correctTyped / totalTyped * 100.0 : 100.0;
    }

    /**
     * @brief Convert to QVariantMap for QML
     * @return QVariantMap representation
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["character"] = QString(character);
        map["totalTyped"] = totalTyped;
        map["correctTyped"] = correctTyped;
        map["incorrectTyped"] = incorrectTyped;
        map["accuracy"] = accuracy();
        return map;
    }
};

/**
 * @brief Detailed character history for typed text visualization
 *
 * Contains complete information about each character position
 * including correction history and word context
 */
struct CharacterHistory {
    int position;               ///< Position in target text
    QChar expected;             ///< Expected character
    QVector<QChar> typedChars;  ///< All characters typed at this position (history)
    QString finalState;         ///< "correct", "incorrect", "corrected", "skipped"
    QChar actualTyped;          ///< What was actually typed (for display)
    int wordIndex;              ///< Which word this character belongs to
    int wordWpm;                ///< WPM for this word
    QString expectedWord;       ///< The full expected word
    int correctionCount;        ///< How many times this character was retyped

    /**
     * @brief Convert to QVariantMap for QML
     * @return QVariantMap representation
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["position"] = position;
        map["expected"] = QString(expected);
        map["actualTyped"] = QString(actualTyped);
        map["finalState"] = finalState;
        map["wordIndex"] = wordIndex;
        map["wordWpm"] = wordWpm;
        map["expectedWord"] = expectedWord;
        map["correctionCount"] = correctionCount;

        QStringList typed;
        for (const QChar& ch : typedChars) {
            typed.append(QString(ch));
        }
        map["typedChars"] = typed;
        return map;
    }
};

/**
 * @brief Overall test statistics
 *
 * Aggregated statistics for the entire typing test
 */
struct TestStatistics {
    int totalWords;           ///< Total words in test
    int correctWords;         ///< Words typed correctly
    int totalCharacters;      ///< Total characters in test
    int correctCharacters;    ///< Characters typed correctly
    int incorrectCharacters;  ///< Characters typed incorrectly
    int skippedCharacters;    ///< Characters skipped
    int correctedCharacters;  ///< Characters retyped after backspace
    double accuracy;          ///< Overall accuracy percentage
    int finalWpm;             ///< Final WPM score
    qint64 totalTime;         ///< Test duration in milliseconds

    /**
     * @brief Convert to QVariantMap for QML
     * @return QVariantMap representation
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["totalWords"] = totalWords;
        map["correctWords"] = correctWords;
        map["totalCharacters"] = totalCharacters;
        map["correctCharacters"] = correctCharacters;
        map["incorrectCharacters"] = incorrectCharacters;
        map["skippedCharacters"] = skippedCharacters;
        map["correctedCharacters"] = correctedCharacters;
        map["accuracy"] = accuracy;
        map["finalWpm"] = finalWpm;
        map["totalTime"] = totalTime;
        return map;
    }
};

/**
 * @brief Test result data collector and analyzer
 *
 * This class collects detailed typing test data during test execution,
 * including WPM history, errors, character statistics, and correction history.
 * After test completion, it analyzes the data to generate comprehensive
 * statistics and visualizations.
 */
class TestResult : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList wpmHistory READ WpmHistory NOTIFY dataChanged)
    Q_PROPERTY(QVariantList errors READ Errors NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap statistics READ Statistics NOTIFY dataChanged)
    Q_PROPERTY(QVariantList characterStats READ CharacterStats NOTIFY dataChanged)
    Q_PROPERTY(
        QVariantList typedTextHistory READ TypedTextHistory NOTIFY dataChanged)
    Q_PROPERTY(bool hasData READ HasData NOTIFY dataChanged)
    Q_PROPERTY(bool isFinalized READ IsFinalized NOTIFY dataChanged)

  public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit TestResult(QObject* parent = nullptr);

    /**
     * @brief Record a WPM data point
     * @param timestamp Milliseconds since test start
     * @param wpm Words per minute at this point
     * @param accuracy Accuracy percentage at this point
     */
    void RecordWpmDataPoint(qint64 timestamp, int wpm, double accuracy);

    /**
     * @brief Record a character typing error
     * @param position Position in target text
     * @param expected Expected character
     * @param actual Character actually typed
     * @param timestamp Milliseconds since test start
     */
    void RecordCharacterError(int position, QChar expected, QChar actual,
                              qint64 timestamp);

    /**
     * @brief Record a typed character
     * @param character The character typed
     * @param correct Whether it was correct
     */
    void RecordCharacterTyped(QChar character, bool correct);

    /**
     * @brief Record a skipped character
     */
    void RecordSkippedCharacter();

    /**
     * @brief Record a correctly completed word
     */
    void RecordCorrectWord();

    /**
     * @brief Record word completion with WPM
     * @param wordIndex Index of completed word
     * @param timestamp Milliseconds since test start
     * @param wpm Current WPM when word was completed
     */
    void RecordWordCompletion(int wordIndex, qint64 timestamp, int wpm);

    /**
     * @brief Record a character correction (backspace/retype)
     * @param position Position that was corrected
     */
    void RecordCharacterCorrection(int position);

    /**
     * @brief Update maximum typed position
     * @param position Furthest position typed so far
     */
    void UpdateMaxTypedPosition(int position);

    /**
     * @brief Finalize and analyze test results
     * @param targetText The target text that was supposed to be typed
     * @param userInput What the user actually typed
     * @param totalTime Test duration in milliseconds
     * @param finalWpm Final WPM score
     * @param finalAccuracy Final accuracy percentage
     */
    void FinalizeResults(const QString& targetText, const QString& userInput,
                         qint64 totalTime, int finalWpm, double finalAccuracy);

    /**
     * @brief Clear all data and reset
     */
    Q_INVOKABLE void Clear();

    /**
     * @brief Get WPM history for charting
     * @return QVariantList of WPM data points
     */
    QVariantList WpmHistory() const;

    /**
     * @brief Get all typing errors
     * @return QVariantList of error records
     */
    QVariantList Errors() const;

    /**
     * @brief Get overall test statistics
     * @return QVariantMap containing statistics
     */
    QVariantMap Statistics() const;

    /**
     * @brief Get character-by-character statistics
     * @return QVariantList of character stats
     */
    QVariantList CharacterStats() const;

    /**
     * @brief Get typed text history for visualization
     * @return QVariantList of character history records
     */
    QVariantList TypedTextHistory() const;

    /**
     * @brief Check if test has any recorded data
     * @return true if data exists, false otherwise
     */
    bool HasData() const { return !m_wpmHistory.isEmpty(); }

    /**
     * @brief Check if results have been finalized
     * @return true if finalized, false otherwise
     */
    bool IsFinalized() const { return m_finalized; }

    /**
     * @brief Get maximum WPM achieved during test
     * @return Maximum WPM value
     */
    Q_INVOKABLE int GetMaxWpm() const;

    /**
     * @brief Get minimum WPM during test
     * @return Minimum WPM value
     */
    Q_INVOKABLE int GetMinWpm() const;

    /**
     * @brief Get average WPM across test
     * @return Average WPM value
     */
    Q_INVOKABLE double GetAverageWpm() const;

    /**
     * @brief Get total error count
     * @return Number of errors
     */
    Q_INVOKABLE int GetErrorCount() const { return m_errors.size(); }

    /**
     * @brief Get most frequently missed characters
     * @param count Number of characters to return
     * @return QVariantList of most missed characters
     */
    Q_INVOKABLE QVariantList GetMostMissedCharacters(int count = 5) const;

  signals:
    void dataChanged();

  private:
    /**
     * @brief Calculate per-character statistics
     */
    void CalculateCharacterStats();

    /**
     * @brief Build detailed character-by-character history
     * @param targetText Target text
     * @param userInput User's typed input
     */
    void BuildTypedTextHistory(const QString& targetText,
                               const QString& userInput);

    QVector<WpmDataPoint> m_wpmHistory;
    QVector<CharacterError> m_errors;
    QMap<QChar, struct CharacterStats> m_characterStatsMap;
    QVector<CharacterHistory> m_typedTextHistory;
    QMap<int, int> m_wordWpmMap;          // wordIndex -> wpm
    QMap<int, int> m_correctionCountMap;  // position -> correction count
    int m_maxTypedPosition;               // Maximum position user has typed to
    TestStatistics m_statistics;
    bool m_finalized;
};

#endif  // TESTRESULT_HPP
