#ifndef TYPINGTEST_HPP
#define TYPINGTEST_HPP

#include <QElapsedTimer>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

#include "TestResult.hpp"
#include "WordGenerator.hpp"

class TestHistory;

/**
 * @brief Core typing test controller
 *
 * This class manages the entire typing test lifecycle, including
 * test initialization, user input processing, live metrics calculation,
 * completion detection, and result finalization. Supports both time-based
 * and word-count-based test modes.
 */
class TypingTest : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString targetText READ targetText NOTIFY targetTextChanged)
    Q_PROPERTY(QString userInput READ userInput WRITE setUserInput NOTIFY
                   userInputChanged)
    Q_PROPERTY(int wpm READ wpm NOTIFY wpmChanged)
    Q_PROPERTY(double accuracy READ accuracy NOTIFY accuracyChanged)
    Q_PROPERTY(int liveWpm READ liveWpm NOTIFY liveWpmChanged)
    Q_PROPERTY(double liveAccuracy READ liveAccuracy NOTIFY liveAccuracyChanged)
    Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged)
    Q_PROPERTY(int remainingTime READ remainingTime NOTIFY remainingTimeChanged)
    Q_PROPERTY(
        int remainingWords READ remainingWords NOTIFY remainingWordsChanged)
    Q_PROPERTY(TestMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(
        int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(
        int wordCount READ wordCount WRITE setWordCount NOTIFY wordCountChanged)
    Q_PROPERTY(
        int totalCharacters READ totalCharacters NOTIFY totalCharactersChanged)
    Q_PROPERTY(int correctCharacters READ correctCharacters NOTIFY
                   correctCharactersChanged)
    Q_PROPERTY(int incorrectCharacters READ incorrectCharacters NOTIFY
                   incorrectCharactersChanged)
    Q_PROPERTY(TestResult* testResult READ testResult CONSTANT)

  public:
    /**
     * @brief Test mode enumeration
     */
    enum TestMode {
        TimeBased,      ///< Test runs for a fixed duration
        WordCountBased  ///< Test runs until word count is reached
    };
    Q_ENUM(TestMode)

    /**
     * @brief Constructor
     * @param wordGenerator Word generator for target text
     * @param parent Parent QObject
     */
    explicit TypingTest(WordGenerator* wordGenerator, QObject* parent = nullptr);

    // Property getters
    QString targetText() const { return m_targetText; }
    QString userInput() const { return m_userInput; }
    int wpm() const { return m_wpm; }
    double accuracy() const { return m_accuracy; }
    int liveWpm() const { return m_liveWpm; }
    double liveAccuracy() const { return m_liveAccuracy; }
    bool isActive() const { return m_isActive; }
    int remainingTime() const { return m_remainingTime; }
    int remainingWords() const { return m_remainingWords; }
    TestMode mode() const { return m_mode; }
    int duration() const { return m_duration; }
    int wordCount() const { return m_wordCount; }
    int totalCharacters() const { return m_totalCharacters; }
    int correctCharacters() const { return m_correctCharacters; }
    int incorrectCharacters() const { return m_incorrectCharacters; }
    TestResult* testResult() const { return m_testResult; }

    /**
     * @brief Set user input text
     * @param input User's typed text
     */
    void setUserInput(const QString& input);

    /**
     * @brief Set test mode
     * @param mode Test mode (time or word count based)
     */
    void setMode(TestMode mode);

    /**
     * @brief Set test duration for time-based mode
     * @param seconds Duration in seconds
     */
    void setDuration(int seconds);

    /**
     * @brief Set word count for word-count-based mode
     * @param count Number of words
     */
    void setWordCount(int count);

    /**
     * @brief Set test history manager for saving results
     * @param testHistory TestHistory instance
     */
    void setTestHistory(TestHistory* testHistory);

    /**
     * @brief Start the typing test
     */
    Q_INVOKABLE void StartTest();

    /**
     * @brief Stop the test before completion
     */
    Q_INVOKABLE void StopTest();

    /**
     * @brief Reset test to initial state
     */
    Q_INVOKABLE void ResetTest();

    /**
     * @brief Get character-by-character states for visualization
     * @return QVariantMap with character state information
     */
    Q_INVOKABLE QVariantMap GetCharacterStates() const;

  signals:
    void targetTextChanged();
    void userInputChanged();
    void wpmChanged();
    void accuracyChanged();
    void liveWpmChanged();
    void liveAccuracyChanged();
    void isActiveChanged();
    void remainingTimeChanged();
    void remainingWordsChanged();
    void modeChanged();
    void durationChanged();
    void wordCountChanged();
    void testCompleted();
    void totalCharactersChanged();
    void correctCharactersChanged();
    void incorrectCharactersChanged();

  private slots:
    /**
     * @brief Update timer countdown (called every second)
     */
    void UpdateTimer();

    /**
     * @brief Update live WPM and accuracy metrics (called periodically)
     */
    void UpdateLiveMetrics();

  private:
    /**
     * @brief Calculate live WPM and accuracy
     */
    void CalculateLiveMetrics();

    /**
     * @brief Generate target text for the test
     */
    void GenerateTargetText();

    /**
     * @brief Check if test completion conditions are met
     */
    void CheckTestCompletion();

    WordGenerator* m_wordGenerator;
    QString m_targetText;
    QStringList m_targetWords;  // Cached split of target text
    QString m_userInput;
    int m_wpm;
    double m_accuracy;
    int m_liveWpm;
    double m_liveAccuracy;
    bool m_isActive;
    int m_remainingTime;
    int m_remainingWords;
    TestMode m_mode;
    int m_duration;
    int m_wordCount;
    int m_totalCharacters;
    int m_correctCharacters;
    int m_incorrectCharacters;

    QElapsedTimer m_elapsedTimer;
    QTimer* m_updateTimer;
    QTimer* m_metricsTimer;
    TestResult* m_testResult;
    TestHistory* m_testHistory;
    qint64 m_lastRecordTime;
    int m_lastProcessedLength;
    int m_lastRecordedWords;
    QSet<int> m_positionsWithErrors;  // Positions that have ever had incorrect characters
};

#endif  // TYPINGTEST_HPP
