#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>
#include <iostream>

#include "TestHistory.hpp"
#include "Version.hpp"

// Generate realistic test data with variety
void populateTestDatabase(TestHistory* testHistory, int numberOfTests = 100) {
    qDebug() << "Populating test database with" << numberOfTests << "tests...";

    // Different test modes and configurations
    QStringList modes = {"time", "words"};
    QList<int> durations = {15, 30, 60, 120};
    QList<int> wordCounts = {10, 25, 50, 100};

    // Performance ranges (WPM and accuracy)
    int minWpm = 40;
    int maxWpm = 120;
    double minAccuracy = 85.0;
    double maxAccuracy = 99.5;

    // Generate tests over the last 15 days
    QDateTime now = QDateTime::currentDateTime();
    int daysToSpread = 15;

    for (int i = 0; i < numberOfTests; ++i) {
        QVariantMap testData;

        // Random mode
        QString mode = modes[QRandomGenerator::global()->bounded(modes.size())];
        testData["mode"] = mode;

        // Random performance with some realism
        // Better performance tends to cluster around 70-90 WPM
        int baseWpm = 70 + QRandomGenerator::global()->bounded(30);
        int variance = QRandomGenerator::global()->bounded(20) - 10;
        int wpm = qBound(minWpm, baseWpm + variance, maxWpm);
        testData["wpm"] = wpm;

        // Accuracy tends to be higher with lower WPM
        double baseAccuracy = 95.0;
        double wpmPenalty =
            (wpm - 70) * 0.1;  // Faster typing = slightly lower accuracy
        double accVariance = (QRandomGenerator::global()->bounded(100) - 50) / 10.0;
        double accuracy = qBound(
            minAccuracy, baseAccuracy - wpmPenalty + accVariance, maxAccuracy);
        testData["accuracy"] = accuracy;

        if (mode == "time") {
            // Time-based test
            int duration =
                durations[QRandomGenerator::global()->bounded(durations.size())];
            testData["duration"] = duration;
            testData["wordCount"] = 0;

            // Calculate characters based on WPM and duration
            int totalChars = (wpm * 5 * duration) / 60;  // 5 chars per word average
            int correctChars = static_cast<int>(totalChars * (accuracy / 100.0));
            int incorrectChars = totalChars - correctChars;

            testData["correctChars"] = correctChars;
            testData["incorrectChars"] = incorrectChars;
            testData["totalChars"] = totalChars;
            testData["totalTime"] = duration * 1000;
        } else {
            // Words-based test
            int words =
                wordCounts[QRandomGenerator::global()->bounded(wordCounts.size())];
            testData["wordCount"] = words;
            testData["duration"] = 0;

            // Calculate characters and time based on word count and WPM
            int totalChars = words * 5;  // 5 chars per word average
            int correctChars = static_cast<int>(totalChars * (accuracy / 100.0));
            int incorrectChars = totalChars - correctChars;
            int totalTime = (words * 60 * 1000) / wpm;  // milliseconds

            testData["correctChars"] = correctChars;
            testData["incorrectChars"] = incorrectChars;
            testData["totalChars"] = totalChars;
            testData["totalTime"] = totalTime;
        }

        // Distribute tests across the last 15 days
        // More recent days have more tests (weighted distribution)
        int daysAgo;
        int rand = QRandomGenerator::global()->bounded(100);
        if (rand < 40) {
            // 40% chance: today or yesterday
            daysAgo = QRandomGenerator::global()->bounded(2);
        } else if (rand < 70) {
            // 30% chance: 2-7 days ago
            daysAgo = 2 + QRandomGenerator::global()->bounded(6);
        } else {
            // 30% chance: 8-15 days ago
            daysAgo = 8 + QRandomGenerator::global()->bounded(8);
        }

        QDateTime testTime = now.addDays(-daysAgo);
        // Add random hours/minutes to spread within the day
        testTime = testTime.addSecs(-QRandomGenerator::global()->bounded(86400));

        testHistory->SaveTest(testData, testTime);

        // Show progress
        if ((i + 1) % 20 == 0) {
            qDebug() << "  Progress:" << (i + 1) << "/" << numberOfTests;
        }
    }

    qDebug() << "Database population complete!";
    qDebug() << "\nStatistics:";
    qDebug() << "  Total Tests:" << testHistory->TotalTests();
    qDebug() << "  Average WPM:" << testHistory->AverageWpm();
    qDebug() << "  Average Accuracy:" << testHistory->AverageAccuracy() << "%";
    qDebug() << "  Highest WPM:" << testHistory->HighestWpm();
    qDebug() << "  Current Streak:" << testHistory->CurrentStreak();
    qDebug() << "  Longest Streak:" << testHistory->LongestStreak();
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // Set application organization and name to use production database
    QCoreApplication::setOrganizationName(APP_ORGANIZATION);
    QCoreApplication::setApplicationName(APP_NAME);

    std::cout << "=================================================\n";
    std::cout << "  FingerCrack Test Database Population Tool\n";
    std::cout << "=================================================\n\n";

    // Parse command line arguments
    int numberOfTests = 100;  // Default
    if (argc > 1) {
        bool ok;
        int userInput = QString(argv[1]).toInt(&ok);
        if (ok && userInput > 0 && userInput <= 1000) {
            numberOfTests = userInput;
        } else {
            std::cerr << "Invalid number of tests. Using default: 100\n";
            std::cerr << "Usage: " << argv[0] << " [number_of_tests]\n";
            std::cerr << "       number_of_tests: 1-1000 (default: 100)\n\n";
        }
    }

    std::cout << "This tool will populate your FingerCrack database\n";
    std::cout << "with " << numberOfTests << " sample test results.\n\n";
    std::cout << "WARNING: This will add tests to your existing database!\n";
    std::cout << "         If you want a fresh database, delete it first.\n\n";

    std::cout << "Do you want to continue? (y/n): ";
    std::string response;
    std::getline(std::cin, response);

    if (response != "y" && response != "Y" && response != "yes") {
        std::cout << "\nOperation cancelled.\n";
        return 0;
    }

    std::cout << "\n";

    // Create TestHistory instance
    TestHistory testHistory(nullptr);

    // Populate with sample data
    populateTestDatabase(&testHistory, numberOfTests);

    std::cout << "\nDatabase location: "
              << testHistory.GetDatabasePath().toStdString() << "\n";
    std::cout << "\nYou can now run FingerCrack to see the populated history!\n";

    return 0;
}
