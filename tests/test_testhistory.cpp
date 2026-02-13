#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QVariantMap>

#include "TestHistory.hpp"

class TestHistoryTest : public ::testing::Test {
  protected:
    TestHistory* testHistory;
    QString testDbPath;

    void SetUp() override {
        // Use unique test database path
        QString configPath =
            QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        testDbPath = configPath + "/tests_unittest.db";

        // Remove old test database if exists
        if (QFile::exists(testDbPath)) {
            QFile::remove(testDbPath);
        }

        // Remove default database connection if exists
        if (QSqlDatabase::contains("test_history")) {
            QSqlDatabase::removeDatabase("test_history");
        }

        testHistory = new TestHistory(nullptr);

        // Clear any existing data
        testHistory->ClearHistory();

        // Populate with sample test data
        populateTestData();
    }

    void TearDown() override {
        delete testHistory;

        // Clean up test database
        if (QFile::exists(testDbPath)) {
            QFile::remove(testDbPath);
        }

        // Remove database connection
        QSqlDatabase::removeDatabase("test_history");
    }

    void populateTestData() {
        // Add 5 sample tests with varying performance

        // Test 1: Good performance (Today)
        QVariantMap test1;
        test1["mode"] = "time";
        test1["duration"] = 30;
        test1["wordCount"] = 0;
        test1["wpm"] = 85;
        test1["accuracy"] = 98.5;
        test1["correctChars"] = 250;
        test1["incorrectChars"] = 4;
        test1["totalChars"] = 254;
        test1["totalTime"] = 30000;
        testHistory->SaveTest(test1);

        // Test 2: Medium performance (Today)
        QVariantMap test2;
        test2["mode"] = "words";
        test2["duration"] = 0;
        test2["wordCount"] = 50;
        test2["wpm"] = 70;
        test2["accuracy"] = 95.0;
        test2["correctChars"] = 200;
        test2["incorrectChars"] = 10;
        test2["totalChars"] = 210;
        test2["totalTime"] = 42000;
        testHistory->SaveTest(test2);

        // Test 3: Excellent performance (Today)
        QVariantMap test3;
        test3["mode"] = "time";
        test3["duration"] = 60;
        test3["wordCount"] = 0;
        test3["wpm"] = 95;
        test3["accuracy"] = 99.2;
        test3["correctChars"] = 480;
        test3["incorrectChars"] = 4;
        test3["totalChars"] = 484;
        test3["totalTime"] = 60000;
        testHistory->SaveTest(test3);

        // Test 4: Low performance (Today)
        QVariantMap test4;
        test4["mode"] = "time";
        test4["duration"] = 15;
        test4["wordCount"] = 0;
        test4["wpm"] = 55;
        test4["accuracy"] = 92.0;
        test4["correctChars"] = 115;
        test4["incorrectChars"] = 10;
        test4["totalChars"] = 125;
        test4["totalTime"] = 15000;
        testHistory->SaveTest(test4);

        // Test 5: Words-based test (Today)
        QVariantMap test5;
        test5["mode"] = "words";
        test5["duration"] = 0;
        test5["wordCount"] = 100;
        test5["wpm"] = 78;
        test5["accuracy"] = 96.5;
        test5["correctChars"] = 410;
        test5["incorrectChars"] = 15;
        test5["totalChars"] = 425;
        test5["totalTime"] = 78000;
        testHistory->SaveTest(test5);
    }
};

// Test: Database initialization
TEST_F(TestHistoryTest, DatabaseInitialization) {
    EXPECT_NE(testHistory, nullptr);
    EXPECT_GT(testHistory->TotalTests(), 0);
}

// Test: Save and retrieve test
TEST_F(TestHistoryTest, SaveAndRetrieveTest) {
    int initialCount = testHistory->TotalTests();

    QVariantMap newTest;
    newTest["mode"] = "time";
    newTest["duration"] = 30;
    newTest["wordCount"] = 0;
    newTest["wpm"] = 80;
    newTest["accuracy"] = 97.5;
    newTest["correctChars"] = 240;
    newTest["incorrectChars"] = 6;
    newTest["totalChars"] = 246;
    newTest["totalTime"] = 30000;

    bool saved = testHistory->SaveTest(newTest);
    EXPECT_TRUE(saved);
    EXPECT_EQ(testHistory->TotalTests(), initialCount + 1);
}

// Test: Get recent tests
TEST_F(TestHistoryTest, GetRecentTests) {
    QVariantList recentTests = testHistory->GetRecentTests(3);
    EXPECT_EQ(recentTests.size(), 3);

    // Check if tests are ordered by timestamp (most recent first)
    if (recentTests.size() >= 2) {
        QVariantMap test1 = recentTests[0].toMap();
        QVariantMap test2 = recentTests[1].toMap();

        QDateTime time1 = test1["timestamp"].toDateTime();
        QDateTime time2 = test2["timestamp"].toDateTime();

        EXPECT_GE(time1, time2);
    }

    // Test pagination with startIndex and count
    QVariantList firstPage = testHistory->GetRecentTests(0, 2);
    EXPECT_EQ(firstPage.size(), 2);

    QVariantList secondPage = testHistory->GetRecentTests(2, 2);
    EXPECT_EQ(secondPage.size(), 2);

    // Verify pagination returns different tests
    if (firstPage.size() > 0 && secondPage.size() > 0) {
        QVariantMap firstTest = firstPage[0].toMap();
        QVariantMap secondTest = secondPage[0].toMap();
        EXPECT_NE(firstTest["id"].toInt(), secondTest["id"].toInt());
    }

    // Test invalid parameters
    QVariantList invalid1 = testHistory->GetRecentTests(-1, 5);
    EXPECT_EQ(invalid1.size(), 0);

    QVariantList invalid2 = testHistory->GetRecentTests(0, 0);
    EXPECT_EQ(invalid2.size(), 0);
}

// Test: Average WPM calculation
TEST_F(TestHistoryTest, AverageWpmCalculation) {
    int avgWpm = testHistory->AverageWpm();
    EXPECT_GT(avgWpm, 0);
    EXPECT_LT(avgWpm, 200);  // Reasonable upper bound

    // Average should be within range of our test data (55-95 WPM)
    EXPECT_GE(avgWpm, 50);
    EXPECT_LE(avgWpm, 100);
}

// Test: Highest WPM tracking
TEST_F(TestHistoryTest, HighestWpmTracking) {
    int highestWpm = testHistory->HighestWpm();
    EXPECT_GT(highestWpm, 0);

    // Should be 95 from our test data
    EXPECT_EQ(highestWpm, 95);
}

// Test: Average accuracy calculation
TEST_F(TestHistoryTest, AverageAccuracyCalculation) {
    double avgAccuracy = testHistory->AverageAccuracy();
    EXPECT_GT(avgAccuracy, 0.0);
    EXPECT_LE(avgAccuracy, 100.0);

    // Should be around 96% based on our test data
    EXPECT_GE(avgAccuracy, 90.0);
    EXPECT_LE(avgAccuracy, 100.0);
}

// Test: Streak calculation
TEST_F(TestHistoryTest, StreakCalculation) {
    int currentStreak = testHistory->CurrentStreak();
    int longestStreak = testHistory->LongestStreak();

    EXPECT_GE(currentStreak, 0);
    EXPECT_GE(longestStreak, 0);
    EXPECT_GE(longestStreak, currentStreak);

    // Since all tests are from today, streak should be at least 1
    EXPECT_GE(currentStreak, 1);
}

// Test: Get statistics with limit
TEST_F(TestHistoryTest, GetStatisticsWithLimit) {
    QVariantMap stats = testHistory->GetAverageStats(3);

    EXPECT_TRUE(stats.contains("avgWpm"));
    EXPECT_TRUE(stats.contains("avgAccuracy"));

    int avgWpm = stats["avgWpm"].toInt();
    double avgAccuracy = stats["avgAccuracy"].toDouble();

    EXPECT_GT(avgWpm, 0);
    EXPECT_GT(avgAccuracy, 0.0);
}

// Test: Get best tests (time-based)
TEST_F(TestHistoryTest, GetBestTestsTimeBased) {
    QVariantList bestTests = testHistory->GetBestTests("time", 0);

    EXPECT_GT(bestTests.size(), 0);

    // Check if sorted by WPM (descending)
    if (bestTests.size() >= 2) {
        QVariantMap test1 = bestTests[0].toMap();
        QVariantMap test2 = bestTests[1].toMap();

        int wpm1 = test1["wpm"].toInt();
        int wpm2 = test2["wpm"].toInt();

        EXPECT_GE(wpm1, wpm2);
    }

    // Best test should be 95 WPM
    if (bestTests.size() > 0) {
        QVariantMap bestTest = bestTests[0].toMap();
        EXPECT_EQ(bestTest["wpm"].toInt(), 95);
    }
}

// Test: Get best tests (words-based)
TEST_F(TestHistoryTest, GetBestTestsWordsBased) {
    QVariantList bestTests = testHistory->GetBestTests("words", 0);

    // Should have at least 2 words-based tests from our setup data
    EXPECT_GE(bestTests.size(), 2);

    // Verify first test is words-based
    if (bestTests.size() > 0) {
        QVariantMap firstTest = bestTests[0].toMap();
        EXPECT_EQ(firstTest["mode"].toString(), QString("words"));
    }
}

// Test: Delete test
TEST_F(TestHistoryTest, DeleteTest) {
    QVariantList tests = testHistory->GetRecentTests(1);
    ASSERT_GT(tests.size(), 0);

    QVariantMap firstTest = tests[0].toMap();
    int testId = firstTest["id"].toInt();

    int initialCount = testHistory->TotalTests();

    bool deleted = testHistory->DeleteTest(testId);
    EXPECT_TRUE(deleted);
    EXPECT_EQ(testHistory->TotalTests(), initialCount - 1);
}

// Test: Clear history
TEST_F(TestHistoryTest, ClearHistory) {
    EXPECT_GT(testHistory->TotalTests(), 0);

    testHistory->ClearHistory();

    EXPECT_EQ(testHistory->TotalTests(), 0);
    EXPECT_EQ(testHistory->CurrentStreak(), 0);
    EXPECT_EQ(testHistory->AverageWpm(), 0);
}

// Test: Total tests count
TEST_F(TestHistoryTest, TotalTestsCount) {
    int totalTests = testHistory->TotalTests();

    // Should have 5 tests from setup
    EXPECT_EQ(totalTests, 5);
}

// Test: Edge case - Empty test data
TEST_F(TestHistoryTest, EmptyTestData) {
    testHistory->ClearHistory();

    EXPECT_EQ(testHistory->TotalTests(), 0);
    EXPECT_EQ(testHistory->AverageWpm(), 0);
    EXPECT_EQ(testHistory->AverageAccuracy(), 0.0);
    EXPECT_EQ(testHistory->HighestWpm(), 0);

    QVariantList recentTests = testHistory->GetRecentTests(10);
    EXPECT_EQ(recentTests.size(), 0);
}

// Test: Edge case - Invalid test data
TEST_F(TestHistoryTest, InvalidTestData) {
    QVariantMap invalidTest;
    // Missing required fields

    bool saved = testHistory->SaveTest(invalidTest);
    // Should handle gracefully (might fail or use defaults)
    // The exact behavior depends on implementation
}
