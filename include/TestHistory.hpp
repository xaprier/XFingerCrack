#ifndef TESTHISTORY_HPP
#define TESTHISTORY_HPP

#include <QDateTime>
#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>
#include <QVariantMap>

/**
 * @brief Test history database manager
 *
 * This class manages persistent storage of typing test results using SQLite.
 * It tracks test history, calculates streaks, provides statistics,
 * and handles filtering/querying of historical data.
 */
class TestHistory : public QObject {
    Q_OBJECT
    Q_PROPERTY(int currentStreak READ CurrentStreak NOTIFY streakChanged)
    Q_PROPERTY(int longestStreak READ LongestStreak NOTIFY streakChanged)
    Q_PROPERTY(int totalTests READ TotalTests NOTIFY totalTestsChanged)
    Q_PROPERTY(int averageWpm READ AverageWpm NOTIFY statsChanged)
    Q_PROPERTY(double averageAccuracy READ AverageAccuracy NOTIFY statsChanged)
    Q_PROPERTY(int highestWpm READ HighestWpm NOTIFY statsChanged)

  public:
    /**
     * @brief Individual test record structure
     */
    struct TestRecord {
        int id;               ///< Database record ID
        QDateTime timestamp;  ///< When test was completed
        QString mode;         ///< "time" or "words"
        int duration;         ///< For time mode (seconds)
        int wordCount;        ///< For words mode
        int wpm;              ///< Words per minute
        double accuracy;      ///< Accuracy percentage
        int correctChars;     ///< Correct character count
        int incorrectChars;   ///< Incorrect character count
        int totalChars;       ///< Total characters typed
        int totalTime;        ///< Test duration in milliseconds
    };

    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit TestHistory(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~TestHistory();

    // Property accessors
    int CurrentStreak() const { return m_currentStreak; }
    int LongestStreak() const { return m_longestStreak; }
    int TotalTests() const { return m_totalTests; }
    int AverageWpm() const { return m_averageWpm; }
    double AverageAccuracy() const { return m_averageAccuracy; }
    int HighestWpm() const { return m_highestWpm; }

    /**
     * @brief Save a test result to database
     * @param testData QVariantMap containing test data
     * @return true if successful, false otherwise
     */
    Q_INVOKABLE bool SaveTest(const QVariantMap& testData);

    /**
     * @brief Save a test result with custom timestamp
     * @param testData QVariantMap containing test data
     * @param customTimestamp Custom timestamp for the test
     * @return true if successful, false otherwise
     */
    bool SaveTest(const QVariantMap& testData, const QDateTime& customTimestamp);

    /**
     * @brief Get recent tests
     * @param limit Maximum number of tests to retrieve
     * @return QVariantList of test records
     */
    Q_INVOKABLE QVariantList GetRecentTests(int limit = 10);

    /**
     * @brief Get recent tests with pagination
     * @param startIndex Starting index
     * @param count Number of tests to retrieve
     * @return QVariantList of test records
     */
    Q_INVOKABLE QVariantList GetRecentTests(int startIndex, int count);

    /**
     * @brief Get recent tests with filters
     * @param startIndex Starting index
     * @param count Number of tests to retrieve
     * @param dateFilter Date filter string
     * @param modeFilter Mode filter ("time" or "words")
     * @param durationFilter Duration/word count filter
     * @return QVariantList of filtered test records
     */
    Q_INVOKABLE QVariantList GetRecentTests(int startIndex, int count,
                                            const QString& dateFilter,
                                            const QString& modeFilter,
                                            int durationFilter);

    /**
     * @brief Get tests from a specific date
     * @param date Date to query
     * @return QVariantList of test records from that date
     */
    Q_INVOKABLE QVariantList GetTestsByDate(const QDate& date);

    /**
     * @brief Get statistics for recent period
     * @param days Number of days to analyze
     * @return QVariantMap containing statistics
     */
    Q_INVOKABLE QVariantMap GetStatistics(int days = 30);

    /**
     * @brief Get average statistics
     * @param limit Number of recent tests to average
     * @return QVariantMap containing average statistics
     */
    Q_INVOKABLE QVariantMap GetAverageStats(int limit = 10);

    /**
     * @brief Get best test results
     * @param mode Test mode filter
     * @param limitValue Duration/word count filter
     * @return QVariantList of best test records
     */
    Q_INVOKABLE QVariantList GetBestTests(const QString& mode,
                                          int limitValue = 0);

    /**
     * @brief Delete a test by ID
     * @param testId Database ID of test to delete
     * @return true if successful, false otherwise
     */
    Q_INVOKABLE bool DeleteTest(int testId);

    /**
     * @brief Clear all test history
     */
    Q_INVOKABLE void ClearHistory();

    /**
     * @brief Get database file path
     * @return Absolute path to SQLite database file
     */
    Q_INVOKABLE QString GetDatabasePath() const;

  signals:
    void streakChanged();
    void totalTestsChanged();
    void statsChanged();
    void historyChanged();

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
     * @brief Calculate current and longest streak
     */
    void CalculateStreaks();

    /**
     * @brief Calculate overall statistics
     */
    void CalculateStats();

    QSqlDatabase m_database;
    int m_currentStreak;
    int m_longestStreak;
    int m_totalTests;
    int m_averageWpm;
    double m_averageAccuracy;
    int m_highestWpm;
};

#endif  // TESTHISTORY_HPP
