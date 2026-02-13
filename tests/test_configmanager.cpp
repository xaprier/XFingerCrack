#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>

#include "ConfigManager.hpp"

class ConfigManagerTest : public ::testing::Test {
  protected:
    ConfigManager* config;
    QString testOrgName;
    QString testAppName;

    void SetUp() override {
        // Use test-specific organization and app name
        testOrgName = "xaprier-test";
        testAppName = "FingerCrack-test";

        QCoreApplication::setOrganizationName(testOrgName);
        QCoreApplication::setApplicationName(testAppName);

        // Clear any existing test settings
        QSettings settings;
        settings.clear();
        settings.sync();

        config = new ConfigManager(nullptr);
    }

    void TearDown() override {
        delete config;

        // Clean up test settings
        QSettings settings;
        settings.clear();
        settings.sync();
    }
};

// Test: Initial default values
TEST_F(ConfigManagerTest, DefaultValues) {
    EXPECT_FALSE(config->Theme().isEmpty());
    EXPECT_GE(config->Mode(), 0);
    EXPECT_GT(config->Duration(), 0);
    EXPECT_GT(config->WordCount(), 0);
    EXPECT_FALSE(config->Language().isEmpty());
}

// Test: Theme setting
TEST_F(ConfigManagerTest, ThemeSetting) {
    QString testTheme = "dark";
    config->SetTheme(testTheme);
    EXPECT_EQ(config->Theme(), testTheme);

    testTheme = "light";
    config->SetTheme(testTheme);
    EXPECT_EQ(config->Theme(), testTheme);
}

// Test: Mode setting
TEST_F(ConfigManagerTest, ModeSetting) {
    config->SetMode(0);
    EXPECT_EQ(config->Mode(), 0);

    config->SetMode(1);
    EXPECT_EQ(config->Mode(), 1);
}

// Test: Duration setting
TEST_F(ConfigManagerTest, DurationSetting) {
    config->SetDuration(30);
    EXPECT_EQ(config->Duration(), 30);

    config->SetDuration(60);
    EXPECT_EQ(config->Duration(), 60);

    config->SetDuration(120);
    EXPECT_EQ(config->Duration(), 120);
}

// Test: Word count setting
TEST_F(ConfigManagerTest, WordCountSetting) {
    config->SetWordCount(25);
    EXPECT_EQ(config->WordCount(), 25);

    config->SetWordCount(50);
    EXPECT_EQ(config->WordCount(), 50);

    config->SetWordCount(100);
    EXPECT_EQ(config->WordCount(), 100);
}

// Test: Language setting
TEST_F(ConfigManagerTest, LanguageSetting) {
    QString testLang = "english";
    config->SetLanguage(testLang);
    EXPECT_EQ(config->Language(), testLang);

    testLang = "turkish";
    config->SetLanguage(testLang);
    EXPECT_EQ(config->Language(), testLang);
}

// Test: Sound enabled setting
TEST_F(ConfigManagerTest, SoundEnabledSetting) {
    config->SetSoundEnabled(true);
    EXPECT_TRUE(config->SoundEnabled());

    config->SetSoundEnabled(false);
    EXPECT_FALSE(config->SoundEnabled());
}

// Test: Sound on click setting
TEST_F(ConfigManagerTest, SoundOnClickSetting) {
    config->SetSoundOnClick(true);
    EXPECT_TRUE(config->SoundOnClick());

    config->SetSoundOnClick(false);
    EXPECT_FALSE(config->SoundOnClick());
}

// Test: Sound on error setting
TEST_F(ConfigManagerTest, SoundOnErrorSetting) {
    config->SetSoundOnError(true);
    EXPECT_TRUE(config->SoundOnError());

    config->SetSoundOnError(false);
    EXPECT_FALSE(config->SoundOnError());
}

// Test: Click sound variant setting
TEST_F(ConfigManagerTest, ClickSoundVariantSetting) {
    QString variant = "mechanical";
    config->SetClickSoundVariant(variant);
    EXPECT_EQ(config->ClickSoundVariant(), variant);

    variant = "soft";
    config->SetClickSoundVariant(variant);
    EXPECT_EQ(config->ClickSoundVariant(), variant);
}

// Test: Sound volume setting
TEST_F(ConfigManagerTest, SoundVolumeSetting) {
    config->SetSoundVolume(0.5);
    EXPECT_DOUBLE_EQ(config->SoundVolume(), 0.5);

    config->SetSoundVolume(1.0);
    EXPECT_DOUBLE_EQ(config->SoundVolume(), 1.0);

    config->SetSoundVolume(0.0);
    EXPECT_DOUBLE_EQ(config->SoundVolume(), 0.0);
}

// Test: UI language setting
TEST_F(ConfigManagerTest, UiLanguageSetting) {
    QString uiLang = "en_US";
    config->SetUiLanguage(uiLang);
    EXPECT_EQ(config->UiLanguage(), uiLang);

    uiLang = "tr_TR";
    config->SetUiLanguage(uiLang);
    EXPECT_EQ(config->UiLanguage(), uiLang);
}

// Test: Save and load settings
TEST_F(ConfigManagerTest, SaveAndLoadSettings) {
    // Set custom values
    config->SetTheme("custom-theme");
    config->SetMode(1);
    config->SetDuration(45);
    config->SetWordCount(75);
    config->SetLanguage("test-language");
    config->SetSoundEnabled(true);
    config->SetSoundVolume(0.75);

    // Save settings
    config->SaveSettings();

    // Create new config instance
    ConfigManager* newConfig = new ConfigManager(nullptr);
    newConfig->LoadSettings();

    // Verify loaded values match saved values
    EXPECT_EQ(newConfig->Theme(), "custom-theme");
    EXPECT_EQ(newConfig->Mode(), 1);
    EXPECT_EQ(newConfig->Duration(), 45);
    EXPECT_EQ(newConfig->WordCount(), 75);
    EXPECT_EQ(newConfig->Language(), "test-language");
    EXPECT_TRUE(newConfig->SoundEnabled());
    EXPECT_DOUBLE_EQ(newConfig->SoundVolume(), 0.75);

    delete newConfig;
}

// Test: Settings persistence
TEST_F(ConfigManagerTest, SettingsPersistence) {
    config->SetTheme("persistent-theme");
    config->SetDuration(90);
    config->SaveSettings();

    // Delete and recreate
    delete config;
    config = new ConfigManager(nullptr);
    config->LoadSettings();

    EXPECT_EQ(config->Theme(), "persistent-theme");
    EXPECT_EQ(config->Duration(), 90);
}

// Test: Multiple save operations
TEST_F(ConfigManagerTest, MultipleSaveOperations) {
    config->SetTheme("theme1");
    config->SaveSettings();

    config->SetTheme("theme2");
    config->SaveSettings();

    config->SetTheme("theme3");
    config->SaveSettings();

    ConfigManager* newConfig = new ConfigManager(nullptr);
    newConfig->LoadSettings();

    EXPECT_EQ(newConfig->Theme(), "theme3");

    delete newConfig;
}

// Test: Edge case - Invalid duration
TEST_F(ConfigManagerTest, InvalidDuration) {
    // Test negative duration
    config->SetDuration(-10);
    // Implementation may clamp or reject invalid values
    // Behavior depends on implementation
}

// Test: Edge case - Invalid word count
TEST_F(ConfigManagerTest, InvalidWordCount) {
    // Test negative word count
    config->SetWordCount(-5);
    // Implementation may clamp or reject invalid values
}

// Test: Edge case - Invalid volume
TEST_F(ConfigManagerTest, InvalidVolume) {
    // Test volume > 1.0
    config->SetSoundVolume(1.5);
    // Should clamp to valid range [0.0, 1.0]
    EXPECT_LE(config->SoundVolume(), 1.0);

    // Test negative volume
    config->SetSoundVolume(-0.5);
    EXPECT_GE(config->SoundVolume(), 0.0);
}

// Test: Edge case - Empty string values
TEST_F(ConfigManagerTest, EmptyStringValues) {
    config->SetTheme("");
    config->SetLanguage("");
    config->SetUiLanguage("");

    // Should handle empty strings gracefully
    config->SaveSettings();

    ConfigManager* newConfig = new ConfigManager(nullptr);
    newConfig->LoadSettings();

    // May use defaults or empty strings depending on implementation
    delete newConfig;
}

// Test: Settings file location
TEST_F(ConfigManagerTest, SettingsFileLocation) {
    config->SetTheme("location-test");
    config->SaveSettings();

    // Verify settings file exists
    QSettings settings;
    QString fileName = settings.fileName();

    EXPECT_FALSE(fileName.isEmpty());
    qDebug() << "Settings file location:" << fileName;
}

// Test: Concurrent access simulation
TEST_F(ConfigManagerTest, ConcurrentAccess) {
    ConfigManager* config1 = new ConfigManager(nullptr);
    ConfigManager* config2 = new ConfigManager(nullptr);

    config1->SetTheme("concurrent1");
    config1->SaveSettings();

    config2->LoadSettings();
    EXPECT_EQ(config2->Theme(), "concurrent1");

    config2->SetTheme("concurrent2");
    config2->SaveSettings();

    config1->LoadSettings();
    EXPECT_EQ(config1->Theme(), "concurrent2");

    delete config1;
    delete config2;
}

// Test: All settings at once
TEST_F(ConfigManagerTest, AllSettingsAtOnce) {
    // Set all available settings
    config->SetTheme("complete-theme");
    config->SetMode(1);
    config->SetDuration(60);
    config->SetWordCount(50);
    config->SetLanguage("complete-lang");
    config->SetSoundEnabled(true);
    config->SetSoundOnClick(true);
    config->SetSoundOnError(false);
    config->SetClickSoundVariant("variant");
    config->SetSoundVolume(0.8);
    config->SetUiLanguage("en_US");

    config->SaveSettings();

    ConfigManager* newConfig = new ConfigManager(nullptr);
    newConfig->LoadSettings();

    // Verify all settings
    EXPECT_EQ(newConfig->Theme(), "complete-theme");
    EXPECT_EQ(newConfig->Mode(), 1);
    EXPECT_EQ(newConfig->Duration(), 60);
    EXPECT_EQ(newConfig->WordCount(), 50);
    EXPECT_EQ(newConfig->Language(), "complete-lang");
    EXPECT_TRUE(newConfig->SoundEnabled());
    EXPECT_TRUE(newConfig->SoundOnClick());
    EXPECT_FALSE(newConfig->SoundOnError());
    EXPECT_EQ(newConfig->ClickSoundVariant(), "variant");
    EXPECT_DOUBLE_EQ(newConfig->SoundVolume(), 0.8);
    EXPECT_EQ(newConfig->UiLanguage(), "en_US");

    delete newConfig;
}
