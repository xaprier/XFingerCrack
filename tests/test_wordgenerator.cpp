#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDebug>
#include <QSet>

#include "WordGenerator.hpp"

class WordGeneratorTest : public ::testing::Test {
  protected:
    WordGenerator* generator;

    void SetUp() override {
        generator = new WordGenerator(nullptr);
        // Load all wordlists from resources
        bool loaded = generator->LoadAllWordlists();
        ASSERT_TRUE(loaded) << "Failed to load wordlists";
    }

    void TearDown() override { delete generator; }
};

// Test: Wordlist loading
TEST_F(WordGeneratorTest, WordlistLoading) {
    QStringList languages = generator->AvailableLanguages();
    EXPECT_GT(languages.size(), 0) << "No languages loaded";

    qDebug() << "Available languages:" << languages;
}

// Test: Current language
TEST_F(WordGeneratorTest, CurrentLanguage) {
    QString currentLang = generator->CurrentLanguage();
    EXPECT_FALSE(currentLang.isEmpty());

    qDebug() << "Current language:" << currentLang;
}

// Test: Word count
TEST_F(WordGeneratorTest, WordCount) {
    int count = generator->WordCount();
    EXPECT_GT(count, 0) << "No words in current language";

    qDebug() << "Word count:" << count;
}

// Test: Generate words
TEST_F(WordGeneratorTest, GenerateWords) {
    int requestedCount = 10;
    QStringList words = generator->GenerateWords(requestedCount);

    EXPECT_EQ(words.size(), requestedCount);

    // Check that all words are non-empty
    for (const QString& word : words) {
        EXPECT_FALSE(word.isEmpty()) << "Generated empty word";
    }

    qDebug() << "Generated words:" << words;
}

// Test: Generate large word list
TEST_F(WordGeneratorTest, GenerateLargeWordList) {
    int requestedCount = 100;
    QStringList words = generator->GenerateWords(requestedCount);

    EXPECT_EQ(words.size(), requestedCount);

    // Verify some randomness (should have some repeated words but not all same)
    QSet<QString> uniqueWords;
    for (const QString& word : words) {
        uniqueWords.insert(word);
    }

    // Should have at least some variety
    EXPECT_GT(uniqueWords.size(), 1) << "All generated words are identical";

    qDebug() << "Generated" << words.size() << "words with" << uniqueWords.size()
             << "unique";
}

// Test: Generate more words than available
TEST_F(WordGeneratorTest, GenerateMoreThanAvailable) {
    int wordCount = generator->WordCount();
    int requestedCount = wordCount + 50;

    QStringList words = generator->GenerateWords(requestedCount);

    // Should still generate requested count (with repetitions)
    EXPECT_EQ(words.size(), requestedCount);
}

// Test: Generate zero words
TEST_F(WordGeneratorTest, GenerateZeroWords) {
    QStringList words = generator->GenerateWords(0);
    EXPECT_EQ(words.size(), 0);
}

// Test: Generate negative word count
TEST_F(WordGeneratorTest, GenerateNegativeWords) {
    QStringList words = generator->GenerateWords(-5);
    EXPECT_EQ(words.size(), 0);
}

// Test: Switch language
TEST_F(WordGeneratorTest, SwitchLanguage) {
    QStringList languages = generator->AvailableLanguages();

    if (languages.size() > 1) {
        QString firstLang = languages[0];
        QString secondLang = languages[1];

        bool loaded = generator->LoadWordlist(secondLang);
        EXPECT_TRUE(loaded);
        EXPECT_EQ(generator->CurrentLanguage(), secondLang);

        // Switch back
        loaded = generator->LoadWordlist(firstLang);
        EXPECT_TRUE(loaded);
        EXPECT_EQ(generator->CurrentLanguage(), firstLang);
    } else {
        GTEST_SKIP() << "Need at least 2 languages to test switching";
    }
}

// Test: Load invalid language
TEST_F(WordGeneratorTest, LoadInvalidLanguage) {
    QString currentLang = generator->CurrentLanguage();

    bool loaded = generator->LoadWordlist("nonexistent_language_xyz");
    EXPECT_FALSE(loaded);

    // Current language should remain unchanged
    EXPECT_EQ(generator->CurrentLanguage(), currentLang);
}

// Test: Randomness of word generation
TEST_F(WordGeneratorTest, WordRandomness) {
    // Generate two sets of words and compare
    QStringList words1 = generator->GenerateWords(20);
    QStringList words2 = generator->GenerateWords(20);

    // They should be different (with very high probability)
    EXPECT_NE(words1, words2) << "Two consecutive word generations are identical";
}

// Test: Word validity (no special characters, proper format)
TEST_F(WordGeneratorTest, WordValidity) {
    QStringList words = generator->GenerateWords(50);

    for (const QString& word : words) {
        // Words should not be empty
        EXPECT_FALSE(word.isEmpty());

        // Words should not start or end with whitespace
        EXPECT_EQ(word, word.trimmed());

        // Words should have reasonable length (1-20 characters)
        EXPECT_GE(word.length(), 1);
        EXPECT_LE(word.length(), 20);
    }
}

// Test: Available languages list
TEST_F(WordGeneratorTest, AvailableLanguagesList) {
    QStringList languages = generator->AvailableLanguages();

    EXPECT_GT(languages.size(), 0);

    // Each language name should be non-empty
    for (const QString& lang : languages) {
        EXPECT_FALSE(lang.isEmpty());
    }

    // Should not have duplicates
    QSet<QString> uniqueLangs(languages.begin(), languages.end());
    EXPECT_EQ(uniqueLangs.size(), languages.size());
}

// Test: Consistency of word generation
TEST_F(WordGeneratorTest, WordGenerationConsistency) {
    // Generate words multiple times and ensure wordCount doesn't change
    int initialCount = generator->WordCount();

    generator->GenerateWords(50);
    EXPECT_EQ(generator->WordCount(), initialCount);

    generator->GenerateWords(100);
    EXPECT_EQ(generator->WordCount(), initialCount);
}
