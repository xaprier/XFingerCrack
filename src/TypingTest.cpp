#include "TypingTest.hpp"

#include <QDebug>
#include <QVariant>
#include <algorithm>

#include "TestHistory.hpp"

TypingTest::TypingTest(WordGenerator* wordGenerator, QObject* parent)
    : QObject(parent), m_wordGenerator(wordGenerator), m_wpm(0), m_accuracy(100.0), m_liveWpm(0), m_liveAccuracy(100.0), m_isActive(false), m_remainingTime(0), m_remainingWords(0), m_mode(TimeBased), m_duration(60), m_wordCount(50), m_totalCharacters(0), m_correctCharacters(0), m_incorrectCharacters(0), m_testHistory(nullptr), m_lastRecordTime(0), m_lastProcessedLength(0), m_lastRecordedWords(0) {
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &TypingTest::UpdateTimer);

    m_metricsTimer = new QTimer(this);
    connect(m_metricsTimer, &QTimer::timeout, this,
            &TypingTest::UpdateLiveMetrics);

    m_testResult = new TestResult(this);
    m_wordStartTimes.clear();
}

void TypingTest::setUserInput(const QString& input) {
    if (m_userInput == input)
        return;

    // Start test on first input
    if (!m_isActive && !input.isEmpty()) {
        StartTest();
    }

    // Track corrections: positions where user previously typed wrong character
    // and now types correct one
    if (m_isActive && !m_targetText.isEmpty() && !input.isEmpty()) {
        int minLen = std::min(input.length(), m_targetText.length());

        for (int i = 0; i < minLen; ++i) {
            bool currentlyCorrect = input[i] == m_targetText[i];
            bool hadErrorBefore = m_positionsWithErrors.contains(i);

            if (currentlyCorrect && hadErrorBefore) {
                // This position was previously wrong, now it's correct - record
                // correction
                m_testResult->RecordCharacterCorrection(i);
                m_positionsWithErrors.remove(i);
            } else if (!currentlyCorrect) {
                // Mark this position as having an error
                m_positionsWithErrors.insert(i);
            }
        }
    }

    m_userInput = input;
    emit userInputChanged();

    if (m_isActive) {
        CheckTestCompletion();
    }
}

void TypingTest::setMode(TestMode mode) {
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged();
}

void TypingTest::setDuration(int seconds) {
    if (m_duration == seconds)
        return;

    m_duration = seconds;
    emit durationChanged();
}

void TypingTest::setWordCount(int count) {
    if (m_wordCount == count)
        return;

    m_wordCount = count;
    emit wordCountChanged();
}

void TypingTest::setTestHistory(TestHistory* testHistory) {
    m_testHistory = testHistory;
}

void TypingTest::StartTest() {
    if (m_isActive)
        return;

    m_testResult->Clear();
    m_lastRecordTime = 0;
    m_lastProcessedLength = 0;
    m_lastRecordedWords = 0;
    m_positionsWithErrors.clear();
    m_wordStartTimes.clear();
    m_wordStartTimes[0] = 0;  // First word starts at time 0
    m_isActive = true;
    m_elapsedTimer.start();

    m_updateTimer->start(100);  // Update every 100ms
    m_metricsTimer->start(
        500);  // Update metrics every 500ms for better granularity

    if (m_mode == TimeBased) {
        m_remainingTime = m_duration;
        emit remainingTimeChanged();
    }

    emit isActiveChanged();
}

void TypingTest::StopTest() {
    if (!m_isActive)
        return;

    // Calculate final metrics BEFORE setting isActive to false
    // (CalculateLiveMetrics checks isActive and returns early if false)
    CalculateLiveMetrics();

    // Record final WPM snapshot
    qint64 totalTime = m_elapsedTimer.elapsed();
    m_testResult->RecordWpmDataPoint(totalTime, m_liveWpm, m_liveAccuracy);

    // Now stop the test
    m_isActive = false;
    m_updateTimer->stop();
    m_metricsTimer->stop();

    // Finalize test results - use live metrics for consistency
    m_testResult->FinalizeResults(m_targetText, m_userInput, totalTime, m_liveWpm,
                                  m_liveAccuracy);

    emit isActiveChanged();
    emit testCompleted();

    // Save test to history
    if (m_testHistory) {
        QVariantMap stats = m_testResult->Statistics();

        QVariantMap testData;
        testData["mode"] = (m_mode == TimeBased) ? "time" : "words";
        testData["duration"] = m_duration;
        testData["wordCount"] = m_wordCount;
        testData["wpm"] = stats["finalWpm"];
        testData["accuracy"] = stats["accuracy"];
        testData["correctChars"] = stats["correctCharacters"];
        testData["incorrectChars"] = stats["incorrectCharacters"];
        testData["totalChars"] = stats["totalCharacters"];
        testData["totalTime"] = totalTime;

        m_testHistory->SaveTest(testData);
    }
}

void TypingTest::ResetTest() {
    StopTest();

    // Clear all state BEFORE emitting userInputChanged to prevent stale data
    m_wpm = 0;
    m_accuracy = 100.0;
    m_liveWpm = 0;
    m_liveAccuracy = 100.0;
    m_totalCharacters = 0;
    m_correctCharacters = 0;
    m_incorrectCharacters = 0;
    m_lastRecordTime = 0;
    m_lastProcessedLength = 0;
    m_lastRecordedWords = 0;
    m_positionsWithErrors.clear();
    m_wordStartTimes.clear();

    // Clear test result before clearing user input
    m_testResult->Clear();

    // Generate new target text before clearing user input
    GenerateTargetText();

    // Clear user input last - this triggers UI updates
    m_userInput.clear();
    emit userInputChanged();

    emit wpmChanged();
    emit accuracyChanged();
    emit liveWpmChanged();
    emit liveAccuracyChanged();
    emit totalCharactersChanged();
    emit correctCharactersChanged();
    emit incorrectCharactersChanged();

    if (m_mode == TimeBased) {
        m_remainingTime = m_duration;
        emit remainingTimeChanged();
    } else {
        m_remainingWords = m_wordCount;
        emit remainingWordsChanged();
    }
}

void TypingTest::GenerateTargetText() {
    int wordsToGenerate = (m_mode == TimeBased) ? 200 : m_wordCount;
    QStringList words = m_wordGenerator->GenerateWords(wordsToGenerate);
    m_targetText = words.join(" ");
    m_targetWords = words;  // Cache the split words

    if (m_mode == WordCountBased) {
        m_remainingWords = m_wordCount;
        emit remainingWordsChanged();
    }

    emit targetTextChanged();
}

void TypingTest::UpdateTimer() {
    if (!m_isActive)
        return;

    if (m_mode == TimeBased) {
        qint64 elapsed = m_elapsedTimer.elapsed();
        int elapsedSeconds = elapsed / 1000;
        m_remainingTime = std::max(0, m_duration - elapsedSeconds);
        emit remainingTimeChanged();

        if (m_remainingTime == 0) {
            StopTest();
        }
    }
}

void TypingTest::UpdateLiveMetrics() {
    if (!m_isActive)
        return;

    CalculateLiveMetrics();

    // Record WPM data point periodically (every 500ms)
    qint64 elapsed = m_elapsedTimer.elapsed();
    if (elapsed - m_lastRecordTime >= 500) {
        m_testResult->RecordWpmDataPoint(elapsed, m_liveWpm, m_liveAccuracy);
        m_lastRecordTime = elapsed;
    }
}

void TypingTest::CalculateLiveMetrics() {
    if (m_targetText.isEmpty() || m_userInput.isEmpty()) {
        m_liveWpm = 0;
        m_liveAccuracy = 100.0;
        emit liveWpmChanged();
        emit liveAccuracyChanged();
        return;
    }

    // Word-based comparison (same logic as finalizeResults)
    QStringList targetWords = m_targetText.split(' ', Qt::SkipEmptyParts);
    QStringList userWords = m_userInput.split(' ', Qt::SkipEmptyParts);

    int correctCharacters = 0;
    int incorrectCharacters = 0;
    int totalCharacters = m_userInput.length();

    // Compare each word character by character
    for (int wordIdx = 0; wordIdx < targetWords.size(); ++wordIdx) {
        QString targetWord = targetWords[wordIdx];
        QString userWord = wordIdx < userWords.size() ? userWords[wordIdx] : "";

        int maxLen = std::max(targetWord.length(), userWord.length());
        for (int charIdx = 0; charIdx < maxLen; ++charIdx) {
            bool hasTarget = charIdx < targetWord.length();
            bool hasUser = charIdx < userWord.length();

            if (hasTarget && hasUser) {
                if (targetWord[charIdx] == userWord[charIdx]) {
                    correctCharacters++;
                } else {
                    incorrectCharacters++;
                }
            } else if (hasUser) {
                // Extra character typed
                incorrectCharacters++;
            }
        }

        // Count space after word (if not last word and user typed it)
        if (wordIdx < targetWords.size() - 1) {
            int spaceCount = 0;
            for (int i = 0; i < m_userInput.length(); ++i) {
                if (m_userInput[i] == ' ')
                    spaceCount++;
            }
            if (wordIdx < spaceCount) {
                correctCharacters++;
            }
        }
    }

    // Update member variables with calculated values
    bool statsChanged = false;
    if (m_correctCharacters != correctCharacters) {
        m_correctCharacters = correctCharacters;
        statsChanged = true;
    }
    if (m_incorrectCharacters != incorrectCharacters) {
        m_incorrectCharacters = incorrectCharacters;
        statsChanged = true;
    }
    if (m_totalCharacters != totalCharacters) {
        m_totalCharacters = totalCharacters;
        statsChanged = true;
    }

    // Calculate accuracy: correct / total * 100
    m_liveAccuracy =
        (totalCharacters > 0)
            ? (static_cast<double>(correctCharacters) / totalCharacters * 100.0)
            : 100.0;

    // Calculate WPM: (total characters / 5) / minutes
    // Use TOTAL elapsed time from start, not just active typing time
    qint64 elapsed = m_elapsedTimer.elapsed();
    if (elapsed > 0 && totalCharacters > 0) {
        double minutes = elapsed / 60000.0;
        double words = totalCharacters / 5.0;
        m_liveWpm = static_cast<int>(words / minutes);
    } else {
        m_liveWpm = 0;
    }

    emit liveWpmChanged();
    emit liveAccuracyChanged();

    if (statsChanged) {
        emit correctCharactersChanged();
        emit incorrectCharactersChanged();
        emit totalCharactersChanged();
    }
}

void TypingTest::CheckTestCompletion() {
    // Update live metrics before checking completion to have accurate WPM values
    CalculateLiveMetrics();

    // Count words completed by counting spaces in user input (for both modes)
    int spaceCount = 0;
    for (int i = 0; i < m_userInput.length(); ++i) {
        if (m_userInput[i] == ' ') {
            spaceCount++;
        }
    }

    // Completed words = spaces only (don't count current word until space is
    // pressed)
    int completedWords = spaceCount;

    // Record newly completed words with word-specific WPM (for both modes)
    if (completedWords > m_lastRecordedWords) {
        // Record start time for the newly started word (if any)
        int nextWordIndex = completedWords;
        if (!m_wordStartTimes.contains(nextWordIndex)) {
            m_wordStartTimes[nextWordIndex] = m_elapsedTimer.elapsed();
        }

        // Process each completed word
        for (int i = m_lastRecordedWords; i < completedWords; ++i) {
            // Get word-specific timing
            qint64 wordStartTime = m_wordStartTimes.value(i, 0);
            qint64 wordEndTime = m_elapsedTimer.elapsed();
            qint64 wordDuration = wordEndTime - wordStartTime;

            // Get actual typed word length
            QStringList userWords = m_userInput.split(' ', Qt::SkipEmptyParts);
            int wordLength = (i < userWords.size()) ? userWords[i].length() : 0;

            // Calculate WPM for this specific word: (characters / 5) / minutes
            int wordWpm = 0;
            if (wordDuration > 0 && wordLength > 0) {
                double minutes = wordDuration / 60000.0;
                wordWpm = static_cast<int>((wordLength / 5.0) / minutes);
            }

            m_testResult->RecordCorrectWord();
            m_testResult->RecordWordCompletion(i, wordEndTime, wordWpm);
        }
        m_lastRecordedWords = completedWords;
    }

    if (m_mode == WordCountBased) {
        m_remainingWords = std::max(0, m_wordCount - completedWords);
        emit remainingWordsChanged();

        // Check if test is complete
        // For N words, we need N-1 spaces + the last word typed
        // So check if we have typed enough words
        if (completedWords >= m_wordCount) {
            StopTest();
        } else if (completedWords == m_wordCount - 1) {
            // Check if last word is being typed (has content after last space)
            // Find position after last space
            int lastSpacePos = m_userInput.lastIndexOf(' ');
            QString lastWord = lastSpacePos >= 0
                                   ? m_userInput.mid(lastSpacePos + 1).trimmed()
                                   : m_userInput.trimmed();

            // Get the target last word
            QStringList targetWords = m_targetText.split(' ', Qt::SkipEmptyParts);
            if (!targetWords.isEmpty() && targetWords.size() == m_wordCount) {
                QString targetLastWord = targetWords[m_wordCount - 1];

                // Complete test if last word matches length (user finished typing it)
                if (lastWord.length() >= targetLastWord.length()) {
                    StopTest();
                }
            }
        }
    }
}

QVariantMap TypingTest::GetCharacterStates() const {
    QVariantMap result;
    QVariantList states;

    int minLength = std::min(m_userInput.length(), m_targetText.length());

    for (int i = 0; i < m_targetText.length(); ++i) {
        QVariantMap charState;
        charState["char"] = QString(m_targetText[i]);

        if (i < minLength) {
            if (m_userInput[i] == m_targetText[i]) {
                charState["state"] = "correct";
            } else {
                charState["state"] = "incorrect";
            }
        } else if (i == minLength) {
            charState["state"] = "current";
        } else {
            charState["state"] = "pending";
        }

        states.append(charState);
    }

    result["characters"] = states;
    return result;
}
