#include "TestResult.hpp"

#include <QDebug>
#include <algorithm>

TestResult::TestResult(QObject* parent)
    : QObject(parent), m_finalized(false), m_maxTypedPosition(0) {
    Clear();
}

void TestResult::RecordWpmDataPoint(qint64 timestamp, int wpm,
                                    double accuracy) {
    WpmDataPoint point;
    point.timestamp = timestamp;
    point.wpm = wpm;
    point.accuracy = accuracy;
    m_wpmHistory.append(point);
    emit dataChanged();
}

void TestResult::RecordCharacterError(int position, QChar expected,
                                      QChar actual, qint64 timestamp) {
    CharacterError error;
    error.position = position;
    error.expected = expected;
    error.actual = actual;
    error.timestamp = timestamp;
    m_errors.append(error);
    emit dataChanged();
}

void TestResult::RecordCharacterTyped(QChar character, bool correct) {
    if (!m_characterStatsMap.contains(character)) {
        struct CharacterStats stats;
        stats.character = character;
        stats.totalTyped = 0;
        stats.correctTyped = 0;
        stats.incorrectTyped = 0;
        m_characterStatsMap[character] = stats;
    }

    struct CharacterStats& stats = m_characterStatsMap[character];
    stats.totalTyped++;
    if (correct) {
        stats.correctTyped++;
        m_statistics.correctCharacters++;
    } else {
        stats.incorrectTyped++;
        m_statistics.incorrectCharacters++;
    }
    m_statistics.totalCharacters++;
}

void TestResult::RecordSkippedCharacter() { m_statistics.skippedCharacters++; }

void TestResult::RecordCorrectWord() { m_statistics.correctWords++; }

void TestResult::RecordWordCompletion(int wordIndex, qint64 timestamp,
                                      int wpm) {
    m_wordWpmMap[wordIndex] = wpm;
}

void TestResult::RecordCharacterCorrection(int position) {
    m_correctionCountMap[position]++;
}

void TestResult::UpdateMaxTypedPosition(int position) {
    if (position > m_maxTypedPosition) {
        m_maxTypedPosition = position;
    }
}

void TestResult::FinalizeResults(const QString& targetText,
                                 const QString& userInput, qint64 totalTime,
                                 int finalWpm, double finalAccuracy) {
    if (m_finalized) {
        return;
    }

    m_statistics.totalTime = totalTime;
    m_statistics.finalWpm = finalWpm;
    m_statistics.accuracy = finalAccuracy;

    // Calculate character statistics using word-based comparison
    m_statistics.totalCharacters = userInput.length();
    m_statistics.correctCharacters = 0;
    m_statistics.incorrectCharacters = 0;

    // Word-based comparison
    QStringList targetWords = targetText.split(' ', Qt::SkipEmptyParts);
    QStringList userWords = userInput.split(' ', Qt::SkipEmptyParts);

    for (int wordIdx = 0; wordIdx < targetWords.size(); ++wordIdx) {
        QString targetWord = targetWords[wordIdx];
        QString userWord = wordIdx < userWords.size() ? userWords[wordIdx] : "";

        int maxLen = std::max(targetWord.length(), userWord.length());
        for (int charIdx = 0; charIdx < maxLen; ++charIdx) {
            bool hasTarget = charIdx < targetWord.length();
            bool hasUser = charIdx < userWord.length();

            if (hasTarget && hasUser) {
                if (targetWord[charIdx] == userWord[charIdx]) {
                    m_statistics.correctCharacters++;
                } else {
                    m_statistics.incorrectCharacters++;
                }
            } else if (hasUser) {
                // Extra character typed
                m_statistics.incorrectCharacters++;
            }
        }

        // Count space after word (if not last word and user typed it)
        if (wordIdx < targetWords.size() - 1) {
            int spaceCount = 0;
            for (int i = 0; i < userInput.length(); ++i) {
                if (userInput[i] == ' ')
                    spaceCount++;
            }
            if (wordIdx < spaceCount) {
                m_statistics.correctCharacters++;
            }
        }
    }

    // Skipped characters = errors that were not corrected (user continued typing
    // despite error) This is already tracked in m_statistics.skippedCharacters
    // via recordSkippedCharacter calls

    // Calculate corrected characters (sum of all corrections)
    m_statistics.correctedCharacters = 0;
    for (auto it = m_correctionCountMap.begin(); it != m_correctionCountMap.end();
         ++it) {
        m_statistics.correctedCharacters += it.value();
    }

    // Count total words safely
    if (!targetText.isEmpty()) {
        QStringList words = targetText.split(" ", Qt::SkipEmptyParts);
        m_statistics.totalWords = words.size();
    } else {
        m_statistics.totalWords = 0;
    }

    // Calculate final WPM and Accuracy using the same logic as live metrics
    if (m_statistics.totalCharacters > 0) {
        m_statistics.accuracy = (double)m_statistics.correctCharacters /
                                m_statistics.totalCharacters * 100.0;
    } else {
        m_statistics.accuracy = 100.0;
    }

    // WPM = (correct characters / 5) / (time in minutes)
    if (m_statistics.totalTime > 0) {
        double minutes = m_statistics.totalTime / 60000.0;
        m_statistics.finalWpm =
            static_cast<int>((m_statistics.correctCharacters / 5.0) / minutes);
    } else {
        m_statistics.finalWpm = 0;
    }

    CalculateCharacterStats();
    BuildTypedTextHistory(targetText, userInput);

    m_finalized = true;

    emit dataChanged();
}

void TestResult::Clear() {
    m_wpmHistory.clear();
    m_errors.clear();
    m_characterStatsMap.clear();
    m_typedTextHistory.clear();
    m_wordWpmMap.clear();
    m_correctionCountMap.clear();
    m_maxTypedPosition = 0;

    m_statistics.totalWords = 0;
    m_statistics.correctWords = 0;
    m_statistics.totalCharacters = 0;
    m_statistics.correctCharacters = 0;
    m_statistics.incorrectCharacters = 0;
    m_statistics.skippedCharacters = 0;
    m_statistics.correctedCharacters = 0;
    m_statistics.accuracy = 0.0;
    m_statistics.finalWpm = 0;
    m_statistics.totalTime = 0;

    m_finalized = false;
    emit dataChanged();
}

QVariantList TestResult::WpmHistory() const {
    QVariantList list;
    for (const auto& point : m_wpmHistory) {
        list.append(point.toVariantMap());
    }
    return list;
}

QVariantList TestResult::Errors() const {
    QVariantList list;
    for (const auto& error : m_errors) {
        list.append(error.toVariantMap());
    }
    return list;
}

QVariantMap TestResult::Statistics() const {
    return m_statistics.toVariantMap();
}

QVariantList TestResult::CharacterStats() const {
    QVariantList list;
    for (const auto& stats : m_characterStatsMap.values()) {
        list.append(stats.toVariantMap());
    }
    return list;
}

QVariantList TestResult::TypedTextHistory() const {
    QVariantList list;
    for (const auto& history : m_typedTextHistory) {
        list.append(history.toVariantMap());
    }
    return list;
}

void TestResult::CalculateCharacterStats() {
    // Character stats are already calculated during typing
    // This method can be used for any final calculations if needed
}

int TestResult::GetMaxWpm() const {
    if (m_wpmHistory.isEmpty()) {
        return 0;
    }

    int maxWpm = 0;
    for (const auto& point : m_wpmHistory) {
        maxWpm = std::max(maxWpm, point.wpm);
    }
    return maxWpm;
}

int TestResult::GetMinWpm() const {
    if (m_wpmHistory.isEmpty()) {
        return 0;
    }

    int minWpm = m_wpmHistory[0].wpm;
    for (const auto& point : m_wpmHistory) {
        if (point.wpm > 0) {  // Ignore zero values
            minWpm = std::min(minWpm, point.wpm);
        }
    }
    return minWpm;
}

double TestResult::GetAverageWpm() const {
    if (m_wpmHistory.isEmpty()) {
        return 0.0;
    }

    qint64 sum = 0;
    int count = 0;
    for (const auto& point : m_wpmHistory) {
        if (point.wpm > 0) {  // Ignore zero values
            sum += point.wpm;
            count++;
        }
    }

    return count > 0 ? (double)sum / count : 0.0;
}

QVariantList TestResult::GetMostMissedCharacters(int count) const {
    // Create a list of character stats sorted by error count
    QList<struct CharacterStats> statsList = m_characterStatsMap.values();

    std::sort(statsList.begin(), statsList.end(),
              [](const struct CharacterStats& a, const struct CharacterStats& b) {
                  return a.incorrectTyped > b.incorrectTyped;
              });

    QVariantList result;
    for (int i = 0; i < std::min(count, static_cast<int>(statsList.size()));
         i++) {
        if (statsList[i].incorrectTyped > 0) {
            result.append(statsList[i].toVariantMap());
        }
    }

    return result;
}

void TestResult::BuildTypedTextHistory(const QString& targetText,
                                       const QString& userInput) {
    m_typedTextHistory.clear();

    // Process character by character to maintain correct positions
    int targetPos = 0;
    int userPos = 0;
    int wordIdx = 0;

    QStringList targetWords = targetText.split(' ', Qt::KeepEmptyParts);
    QStringList userWords = userInput.split(' ', Qt::KeepEmptyParts);

    for (int realPos = 0; realPos < targetText.length(); ++realPos) {
        CharacterHistory history;
        history.position = realPos;

        QChar expectedChar = targetText[realPos];
        QChar actualChar = (realPos < userInput.length()) ? userInput[realPos] : QChar(' ');

        history.expected = expectedChar;
        history.actualTyped = actualChar;

        // Find which word this position belongs to
        int charCount = 0;
        int currentWordIdx = 0;
        for (int w = 0; w < targetWords.size(); ++w) {
            int wordLen = targetWords[w].length();
            if (realPos >= charCount && realPos < charCount + wordLen) {
                currentWordIdx = w;
                break;
            }
            charCount += wordLen;
            if (w < targetWords.size() - 1) {
                charCount++;  // space
                if (realPos == charCount - 1) {
                    currentWordIdx = w;
                    break;
                }
            }
        }

        history.wordIndex = currentWordIdx;
        if (currentWordIdx < targetWords.size()) {
            history.expectedWord = targetWords[currentWordIdx];
        }
        history.wordWpm = m_wordWpmMap.contains(currentWordIdx) ? m_wordWpmMap[currentWordIdx] : m_statistics.finalWpm;

        // Get correction count for this position
        history.correctionCount = m_correctionCountMap.value(realPos, 0);

        // Determine final state
        if (realPos < userInput.length()) {
            if (actualChar == expectedChar) {
                if (history.correctionCount > 0) {
                    history.finalState = "corrected";
                } else {
                    history.finalState = "correct";
                }
            } else {
                history.finalState = "incorrect";
            }
        } else {
            history.finalState = "skipped";
        }

        history.typedChars.append(actualChar);
        m_typedTextHistory.append(history);
    }
}
