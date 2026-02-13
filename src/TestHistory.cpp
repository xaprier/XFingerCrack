#include "TestHistory.hpp"

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

#include "Version.hpp"

TestHistory::TestHistory(QObject* parent)
    : QObject(parent), m_currentStreak(0), m_longestStreak(0), m_totalTests(0), m_averageWpm(0), m_averageAccuracy(0.0), m_highestWpm(0) {
    if (InitDatabase()) {
        CalculateStreaks();
        CalculateStats();
    }
}

TestHistory::~TestHistory() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

QString TestHistory::GetDatabasePath() const {
    QString configPath =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString appName = QString(APP_NAME);

    return configPath + "/" + appName + ".db";
}

bool TestHistory::InitDatabase() {
    QString dbPath = GetDatabasePath();
    qDebug() << "TestHistory: Initializing database at" << dbPath;

    m_database = QSqlDatabase::addDatabase("QSQLITE", "test_history");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qWarning() << "TestHistory: Failed to open database:"
                   << m_database.lastError().text();
        return false;
    }

    return CreateTables();
}

bool TestHistory::CreateTables() {
    QSqlQuery query(m_database);

    // Create tests table
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS tests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp INTEGER NOT NULL,
            mode TEXT NOT NULL,
            duration INTEGER,
            word_count INTEGER,
            wpm INTEGER NOT NULL,
            accuracy REAL NOT NULL,
            correct_chars INTEGER NOT NULL,
            incorrect_chars INTEGER NOT NULL,
            total_chars INTEGER NOT NULL,
            total_time INTEGER NOT NULL
        )
    )";

    if (!query.exec(createTableSQL)) {
        qWarning() << "TestHistory: Failed to create tests table:"
                   << query.lastError().text();
        return false;
    }

    // Create index on timestamp for faster queries
    QString createIndexSQL =
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON tests(timestamp DESC)";
    if (!query.exec(createIndexSQL)) {
        qWarning() << "TestHistory: Failed to create index:"
                   << query.lastError().text();
    }

    qDebug() << "TestHistory: Database tables created successfully";
    return true;
}

bool TestHistory::SaveTest(const QVariantMap& testData) {
    return SaveTest(testData, QDateTime::currentDateTime());
}

bool TestHistory::SaveTest(const QVariantMap& testData,
                           const QDateTime& customTimestamp) {
    QSqlQuery query(m_database);

    QString insertSQL = R"(
        INSERT INTO tests (timestamp, mode, duration, word_count, wpm, accuracy, 
                          correct_chars, incorrect_chars, total_chars, total_time)
        VALUES (:timestamp, :mode, :duration, :word_count, :wpm, :accuracy,
                :correct_chars, :incorrect_chars, :total_chars, :total_time)
    )";

    query.prepare(insertSQL);
    query.bindValue(":timestamp", customTimestamp.toSecsSinceEpoch());
    query.bindValue(":mode", testData.value("mode").toString());
    query.bindValue(":duration", testData.value("duration", 0).toInt());
    query.bindValue(":word_count", testData.value("wordCount", 0).toInt());
    query.bindValue(":wpm", testData.value("wpm").toInt());
    query.bindValue(":accuracy", testData.value("accuracy").toDouble());
    query.bindValue(":correct_chars", testData.value("correctChars").toInt());
    query.bindValue(":incorrect_chars", testData.value("incorrectChars").toInt());
    query.bindValue(":total_chars", testData.value("totalChars").toInt());
    query.bindValue(":total_time", testData.value("totalTime").toInt());

    if (!query.exec()) {
        qWarning() << "TestHistory: Failed to save test:"
                   << query.lastError().text();
        return false;
    }

    qDebug() << "TestHistory: Test saved successfully";

    // Update statistics
    CalculateStreaks();
    CalculateStats();
    emit historyChanged();

    return true;
}

QVariantList TestHistory::GetRecentTests(int limit) {
    QVariantList results;
    QSqlQuery query(m_database);

    QString selectSQL = R"(
        SELECT id, timestamp, mode, duration, word_count, wpm, accuracy,
               correct_chars, incorrect_chars, total_chars, total_time
        FROM tests
        ORDER BY timestamp DESC
        LIMIT :limit
    )";

    query.prepare(selectSQL);
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        qWarning() << "TestHistory: Failed to get recent tests:"
                   << query.lastError().text();
        return results;
    }

    while (query.next()) {
        QVariantMap test;
        test["id"] = query.value(0).toInt();
        test["timestamp"] =
            QDateTime::fromSecsSinceEpoch(query.value(1).toLongLong());
        test["mode"] = query.value(2).toString();
        test["duration"] = query.value(3).toInt();
        test["wordCount"] = query.value(4).toInt();
        test["wpm"] = query.value(5).toInt();
        test["accuracy"] = query.value(6).toDouble();
        test["correctChars"] = query.value(7).toInt();
        test["incorrectChars"] = query.value(8).toInt();
        test["totalChars"] = query.value(9).toInt();
        test["totalTime"] = query.value(10).toInt();
        results.append(test);
    }

    return results;
}

QVariantList TestHistory::GetRecentTests(int startIndex, int count) {
    return GetRecentTests(startIndex, count, "all", "all", 0);
}

QVariantList TestHistory::GetRecentTests(int startIndex, int count,
                                         const QString& dateFilter,
                                         const QString& modeFilter,
                                         int durationFilter) {
    QVariantList results;

    // Validate parameters
    if (startIndex < 0 || count <= 0) {
        qWarning() << "TestHistory: Invalid parameters - startIndex:" << startIndex
                   << "count:" << count;
        return results;
    }

    QSqlQuery query(m_database);

    QString selectSQL = R"(
        SELECT id, timestamp, mode, duration, word_count, wpm, accuracy,
               correct_chars, incorrect_chars, total_chars, total_time
        FROM tests
        WHERE 1=1
    )";

    // Date filter
    if (dateFilter == "today") {
        selectSQL +=
            " AND DATE(timestamp, 'unixepoch', 'localtime') = DATE('now', "
            "'localtime')";
    } else if (dateFilter == "yesterday") {
        selectSQL +=
            " AND DATE(timestamp, 'unixepoch', 'localtime') = DATE('now', "
            "'localtime', '-1 day')";
    } else if (dateFilter == "week") {
        selectSQL +=
            " AND DATE(timestamp, 'unixepoch', 'localtime') >= "
            "DATE('now', 'localtime', '-7 days')";
    } else if (dateFilter == "month") {
        selectSQL +=
            " AND DATE(timestamp, 'unixepoch', 'localtime') >= "
            "DATE('now', 'localtime', '-30 days')";
    }
    // "all" means no date filter

    // Mode filter
    if (modeFilter == "time") {
        selectSQL += " AND mode = 'time'";
    } else if (modeFilter == "words") {
        selectSQL += " AND mode = 'words'";
    }
    // "all" means no mode filter

    // Duration/WordCount filter
    if (durationFilter > 0) {
        selectSQL += " AND (duration = :duration OR word_count = :word_count)";
    }

    selectSQL += R"(
        ORDER BY timestamp DESC
        LIMIT :count OFFSET :offset
    )";

    query.prepare(selectSQL);
    if (durationFilter > 0) {
        query.bindValue(":duration", durationFilter);
        query.bindValue(":word_count", durationFilter);
    }
    query.bindValue(":count", count);
    query.bindValue(":offset", startIndex);

    if (!query.exec()) {
        qWarning() << "TestHistory: Failed to get recent tests:"
                   << query.lastError().text();
        return results;
    }

    while (query.next()) {
        QVariantMap test;
        test["id"] = query.value(0).toInt();
        test["timestamp"] =
            QDateTime::fromSecsSinceEpoch(query.value(1).toLongLong());
        test["mode"] = query.value(2).toString();
        test["duration"] = query.value(3).toInt();
        test["wordCount"] = query.value(4).toInt();
        test["wpm"] = query.value(5).toInt();
        test["accuracy"] = query.value(6).toDouble();
        test["correctChars"] = query.value(7).toInt();
        test["incorrectChars"] = query.value(8).toInt();
        test["totalChars"] = query.value(9).toInt();
        test["totalTime"] = query.value(10).toInt();
        results.append(test);
    }

    qDebug() << "TestHistory: Retrieved" << results.length() << "tests from index"
             << startIndex << "with filters - date:" << dateFilter
             << "mode:" << modeFilter << "duration:" << durationFilter;
    return results;
}

QVariantList TestHistory::GetTestsByDate(const QDate& date) {
    QVariantList results;
    QSqlQuery query(m_database);

    // Get start and end of day as Unix timestamps
    QDateTime startOfDay = QDateTime(date, QTime(0, 0, 0));
    QDateTime endOfDay = QDateTime(date, QTime(23, 59, 59));

    QString selectSQL = R"(
        SELECT id, timestamp, mode, duration, word_count, wpm, accuracy,
               correct_chars, incorrect_chars, total_chars, total_time
        FROM tests
        WHERE timestamp >= :start_time AND timestamp <= :end_time
        ORDER BY timestamp DESC
    )";

    query.prepare(selectSQL);
    query.bindValue(":start_time", startOfDay.toSecsSinceEpoch());
    query.bindValue(":end_time", endOfDay.toSecsSinceEpoch());

    if (!query.exec()) {
        qWarning() << "TestHistory: Failed to get tests by date:"
                   << query.lastError().text();
        return results;
    }

    while (query.next()) {
        QVariantMap test;
        test["id"] = query.value(0).toInt();
        test["timestamp"] =
            QDateTime::fromSecsSinceEpoch(query.value(1).toLongLong());
        test["mode"] = query.value(2).toString();
        test["duration"] = query.value(3).toInt();
        test["wordCount"] = query.value(4).toInt();
        test["wpm"] = query.value(5).toInt();
        test["accuracy"] = query.value(6).toDouble();
        test["correctChars"] = query.value(7).toInt();
        test["incorrectChars"] = query.value(8).toInt();
        test["totalChars"] = query.value(9).toInt();
        test["totalTime"] = query.value(10).toInt();
        results.append(test);
    }

    return results;
}

QVariantMap TestHistory::GetStatistics(int days) {
    QVariantMap stats;
    QSqlQuery query(m_database);

    // Get stats for last N days
    QDateTime startDate = QDateTime::currentDateTime().addDays(-days);

    QString statsSQL = R"(
        SELECT 
            COUNT(*) as test_count,
            AVG(wpm) as avg_wpm,
            MAX(wpm) as max_wpm,
            AVG(accuracy) as avg_accuracy,
            MAX(accuracy) as max_accuracy,
            SUM(total_time) as total_time
        FROM tests
        WHERE timestamp >= :start_time
    )";

    query.prepare(statsSQL);
    query.bindValue(":start_time", startDate.toSecsSinceEpoch());

    if (!query.exec() || !query.next()) {
        qWarning() << "TestHistory: Failed to get statistics:"
                   << query.lastError().text();
        return stats;
    }

    stats["testCount"] = query.value(0).toInt();
    stats["avgWpm"] = query.value(1).toInt();
    stats["maxWpm"] = query.value(2).toInt();
    stats["avgAccuracy"] = query.value(3).toDouble();
    stats["maxAccuracy"] = query.value(4).toDouble();
    stats["totalTime"] = query.value(5).toLongLong();
    stats["days"] = days;

    return stats;
}

void TestHistory::CalculateStreaks() {
    QSqlQuery query(m_database);

    // Get all test dates (grouped by day)
    QString datesSQL = R"(
        SELECT DISTINCT DATE(timestamp, 'unixepoch', 'localtime') as test_date
        FROM tests
        ORDER BY test_date DESC
    )";

    if (!query.exec(datesSQL)) {
        qWarning() << "TestHistory: Failed to calculate streaks:"
                   << query.lastError().text();
        return;
    }

    QList<QDate> testDates;
    while (query.next()) {
        QDate date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
        testDates.append(date);
    }

    if (testDates.isEmpty()) {
        m_currentStreak = 0;
        m_longestStreak = 0;
        emit streakChanged();
        return;
    }

    // Calculate current streak
    m_currentStreak = 0;
    QDate today = QDate::currentDate();
    QDate yesterday = today.addDays(-1);

    // Check if there's a test today or yesterday
    if (testDates.contains(today)) {
        QDate checkDate = today;
        while (testDates.contains(checkDate)) {
            m_currentStreak++;
            checkDate = checkDate.addDays(-1);
        }
    } else if (testDates.contains(yesterday)) {
        QDate checkDate = yesterday;
        while (testDates.contains(checkDate)) {
            m_currentStreak++;
            checkDate = checkDate.addDays(-1);
        }
    }

    // Calculate longest streak
    m_longestStreak = 0;
    int currentStreakCount = 1;

    for (int i = 0; i < testDates.size() - 1; i++) {
        if (testDates[i].daysTo(testDates[i + 1]) == -1) {
            currentStreakCount++;
        } else {
            if (currentStreakCount > m_longestStreak) {
                m_longestStreak = currentStreakCount;
            }
            currentStreakCount = 1;
        }
    }

    if (currentStreakCount > m_longestStreak) {
        m_longestStreak = currentStreakCount;
    }

    qDebug() << "TestHistory: Current streak:" << m_currentStreak
             << "Longest:" << m_longestStreak;
    emit streakChanged();
}

void TestHistory::CalculateStats() {
    QSqlQuery query(m_database);

    // First, let's see what's in the database
    QSqlQuery debugQuery(m_database);
    debugQuery.exec("SELECT id, wpm, accuracy FROM tests");
    qDebug() << "TestHistory::calculateStats - Database contents:";
    while (debugQuery.next()) {
        qDebug() << "  ID:" << debugQuery.value(0).toInt()
                 << "WPM:" << debugQuery.value(1).toInt()
                 << "Accuracy:" << debugQuery.value(2).toDouble();
    }

    // Get overall statistics
    QString statsSQL = R"(
        SELECT 
            COUNT(*) as test_count,
            AVG(wpm) as avg_wpm,
            AVG(accuracy) as avg_accuracy,
            MAX(wpm) as max_wpm
        FROM tests
    )";

    if (!query.exec(statsSQL) || !query.next()) {
        qWarning() << "TestHistory: Failed to calculate stats:"
                   << query.lastError().text();
        return;
    }

    m_totalTests = query.value(0).toInt();
    m_averageWpm = query.value(1).toInt();
    m_averageAccuracy = query.value(2).toDouble();
    m_highestWpm = query.value(3).toInt();

    qDebug() << "TestHistory: Stats - Total tests:" << m_totalTests
             << "Avg WPM:" << m_averageWpm << "Avg Accuracy:" << m_averageAccuracy
             << "Highest WPM:" << m_highestWpm;

    emit totalTestsChanged();
    emit statsChanged();
}

void TestHistory::ClearHistory() {
    QSqlQuery query(m_database);

    if (!query.exec("DELETE FROM tests")) {
        qWarning() << "TestHistory: Failed to clear history:"
                   << query.lastError().text();
        return;
    }

    m_currentStreak = 0;
    m_longestStreak = 0;
    m_totalTests = 0;
    m_averageWpm = 0;
    m_averageAccuracy = 0.0;
    m_highestWpm = 0;

    emit streakChanged();
    emit totalTestsChanged();
    emit statsChanged();
    emit historyChanged();

    qDebug() << "TestHistory: History cleared";
}

QVariantMap TestHistory::GetAverageStats(int limit) {
    QVariantMap stats;
    QSqlQuery query(m_database);

    QString statsSQL = R"(
        SELECT 
            COUNT(*) as test_count,
            AVG(wpm) as avg_wpm,
            AVG(accuracy) as avg_accuracy,
            SUM(total_time) as total_time
        FROM (
            SELECT * FROM tests 
            ORDER BY timestamp DESC 
            LIMIT :limit
        )
    )";

    query.prepare(statsSQL);
    query.bindValue(":limit", limit);

    if (!query.exec() || !query.next()) {
        qWarning() << "TestHistory: Failed to get average stats:"
                   << query.lastError().text();
        return stats;
    }

    stats["testCount"] = query.value(0).toInt();
    stats["avgWpm"] = query.value(1).toInt();
    stats["avgAccuracy"] = query.value(2).toDouble();
    stats["totalTime"] = query.value(3).toLongLong();

    return stats;
}

QVariantList TestHistory::GetBestTests(const QString& mode, int limitValue) {
    QVariantList results;
    QSqlQuery query(m_database);

    QString selectSQL;
    if (limitValue > 0) {
        // Get best tests for specific mode and duration/wordCount
        if (mode == "time") {
            selectSQL = R"(
                SELECT id, timestamp, mode, duration, word_count, wpm, accuracy,
                       correct_chars, incorrect_chars, total_chars, total_time
                FROM tests
                WHERE mode = :mode AND duration = :limit_value
                ORDER BY wpm DESC
                LIMIT 10
            )";
        } else {
            selectSQL = R"(
                SELECT id, timestamp, mode, duration, word_count, wpm, accuracy,
                       correct_chars, incorrect_chars, total_chars, total_time
                FROM tests
                WHERE mode = :mode AND word_count = :limit_value
                ORDER BY wpm DESC
                LIMIT 10
            )";
        }
        query.prepare(selectSQL);
        query.bindValue(":mode", mode);
        query.bindValue(":limit_value", limitValue);
    } else {
        // Get best tests for mode only
        selectSQL = R"(
            SELECT id, timestamp, mode, duration, word_count, wpm, accuracy,
                   correct_chars, incorrect_chars, total_chars, total_time
            FROM tests
            WHERE mode = :mode
            ORDER BY wpm DESC
            LIMIT 10
        )";
        query.prepare(selectSQL);
        query.bindValue(":mode", mode);
    }

    if (!query.exec()) {
        qWarning() << "TestHistory: Failed to get best tests:"
                   << query.lastError().text();
        return results;
    }

    while (query.next()) {
        QVariantMap test;
        test["id"] = query.value(0).toInt();
        test["timestamp"] =
            QDateTime::fromSecsSinceEpoch(query.value(1).toLongLong());
        test["mode"] = query.value(2).toString();
        test["duration"] = query.value(3).toInt();
        test["wordCount"] = query.value(4).toInt();
        test["wpm"] = query.value(5).toInt();
        test["accuracy"] = query.value(6).toDouble();
        test["correctChars"] = query.value(7).toInt();
        test["incorrectChars"] = query.value(8).toInt();
        test["totalChars"] = query.value(9).toInt();
        test["totalTime"] = query.value(10).toInt();
        results.append(test);
    }

    return results;
}

bool TestHistory::DeleteTest(int testId) {
    QSqlQuery query(m_database);

    QString deleteSQL = "DELETE FROM tests WHERE id = :id";
    query.prepare(deleteSQL);
    query.bindValue(":id", testId);

    if (!query.exec()) {
        qWarning() << "TestHistory: Failed to delete test:"
                   << query.lastError().text();
        return false;
    }

    qDebug() << "TestHistory: Test" << testId << "deleted";

    // Update statistics
    CalculateStreaks();
    CalculateStats();
    emit historyChanged();

    return true;
}
