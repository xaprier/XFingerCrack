#include "XFingerCrackApplication.hpp"

#include <QIcon>

#include "AppInfo.hpp"
#include "ConfigManager.hpp"
#include "FontManager.hpp"
#include "TestHistory.hpp"
#include "ThemeManager.hpp"
#include "TranslationManager.hpp"
#include "TypingTest.hpp"
#include "Version.hpp"
#include "WordGenerator.hpp"

XFingerCrackApplication::XFingerCrackApplication(int& argc, char** argv)
    : AbstractQmlApplication(argc, argv), m_configManager(nullptr), m_translationManager(nullptr), m_appInfo(nullptr), m_wordGenerator(nullptr), m_typingTest(nullptr), m_themeManager(nullptr), m_testHistory(nullptr) {
    // Set application metadata
    setOrganizationName(APP_ORGANIZATION);
    setApplicationName(APP_NAME);
    
    // Set application icon
    setWindowIcon(QIcon(":/XFingerCrack.png"));
}

XFingerCrackApplication::~XFingerCrackApplication() {
    // Components will be automatically deleted as children of QApplication
}

void XFingerCrackApplication::initializeComponents() {
    // Create config manager first to load settings
    m_configManager = new ConfigManager(this);

    // Create translation manager
    m_translationManager = new TranslationManager(this);

    // Load saved UI language
    m_translationManager->SetCurrentLanguage(m_configManager->UiLanguage());

    // Connect translation manager to QML engine for retranslation
    m_translationManager->SetEngine(engine());

    // Create app info
    m_appInfo = new AppInfo(this);

    // Create word generator and load wordlists
    m_wordGenerator = new WordGenerator(this);
    m_wordGenerator->LoadAllWordlists();

    // Load saved language
    m_wordGenerator->LoadWordlist(m_configManager->Language());

    // Create typing test
    m_typingTest = new TypingTest(m_wordGenerator, this);

    // Create theme manager and load themes
    m_themeManager = new ThemeManager(this);
    m_themeManager->LoadThemes(":/themes.json");

    // Load saved theme
    m_themeManager->SetTheme(m_configManager->Theme());

    // Create font manager and load fonts
    m_fontManager = new FontManager(this);
    m_fontManager->SetFont(m_configManager->FontFamily());

    // Create test history
    m_testHistory = new TestHistory(this);

    // Connect test history to typing test
    m_typingTest->setTestHistory(m_testHistory);

    // Load saved test settings
    m_typingTest->setMode(static_cast<TypingTest::TestMode>(m_configManager->Mode()));
    m_typingTest->setDuration(m_configManager->Duration());
    m_typingTest->setWordCount(m_configManager->WordCount());

    // Initialize the test with target text
    m_typingTest->ResetTest();
}

void XFingerCrackApplication::registerQmlTypes() {
    // Register enum for QML
    qmlRegisterUncreatableType<TypingTest>(APP_NAME, 1, 0, "TestMode",
                                           "TestMode is an enum");
}

void XFingerCrackApplication::setupContextProperties() {
    // Expose C++ objects to QML
    rootContext()->setContextProperty("typingTest", m_typingTest);
    rootContext()->setContextProperty("themeManager", m_themeManager);
    rootContext()->setContextProperty("wordGenerator", m_wordGenerator);
    rootContext()->setContextProperty("configManager", m_configManager);
    rootContext()->setContextProperty("translationManager", m_translationManager);
    rootContext()->setContextProperty("appInfo", m_appInfo);
    rootContext()->setContextProperty("testHistory", m_testHistory);
    rootContext()->setContextProperty("fontManager", m_fontManager);
}

void XFingerCrackApplication::setupConnections() {
    // Connect UI language changes to translation manager
    QObject::connect(m_configManager, &ConfigManager::uiLanguageChanged,
                     this, [this]() {
                         m_translationManager->SetCurrentLanguage(
                             m_configManager->UiLanguage());
                     });

    // Connect language changes to word generator and typing test
    QObject::connect(m_configManager, &ConfigManager::languageChanged,
                     this, [this]() {
                         m_wordGenerator->LoadWordlist(m_configManager->Language());
                         m_typingTest->ResetTest();
                     });

    // Connect font family changes to font manager
    QObject::connect(m_configManager, &ConfigManager::fontFamilyChanged,
                     this, [this]() {
                         m_fontManager->SetFont(m_configManager->FontFamily());
                     });
}

QUrl XFingerCrackApplication::getMainQmlPath() const {
    return QUrl(QStringLiteral("qrc:/qml/main.qml"));
}

void XFingerCrackApplication::cleanup() {
    // Disconnect all connections to avoid dangling pointers
    if (m_configManager) {
        disconnect(m_configManager, nullptr, nullptr, nullptr);
    }
    if (m_fontManager) {
        disconnect(m_fontManager, nullptr, nullptr, nullptr);
    }

    // Clear context properties before engine destruction
    if (engine() && engine()->rootContext()) {
        engine()->rootContext()->setContextProperty("typingTest", nullptr);
        engine()->rootContext()->setContextProperty("themeManager", nullptr);
        engine()->rootContext()->setContextProperty("wordGenerator", nullptr);
        engine()->rootContext()->setContextProperty("configManager", nullptr);
        engine()->rootContext()->setContextProperty("translationManager", nullptr);
        engine()->rootContext()->setContextProperty("appInfo", nullptr);
        engine()->rootContext()->setContextProperty("testHistory", nullptr);
        engine()->rootContext()->setContextProperty("fontManager", nullptr);
    }
}
