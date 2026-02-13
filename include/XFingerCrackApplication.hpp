#ifndef XFINGERCRACKAPPLICATION_HPP
#define XFINGERCRACKAPPLICATION_HPP

#include "AbstractQmlApplication.hpp"

// Forward declarations
class ConfigManager;
class TranslationManager;
class AppInfo;
class WordGenerator;
class TypingTest;
class ThemeManager;
class TestHistory;

/**
 * @brief FingerCrack application implementation
 *
 * This class implements the FingerCrack typing test application
 * by extending AbstractQmlApplication with application-specific
 * components and logic.
 */
class XFingerCrackApplication : public AbstractQmlApplication {
    Q_OBJECT

  public:
    /**
     * @brief Constructor
     * @param argc Argument count
     * @param argv Argument values
     */
    XFingerCrackApplication(int& argc, char** argv);

    /**
     * @brief Destructor
     */
    ~XFingerCrackApplication() override;

  protected:
    /**
     * @brief Initialize application components
     */
    void initializeComponents() override;

    /**
     * @brief Register QML types
     */
    void registerQmlTypes() override;

    /**
     * @brief Setup context properties
     */
    void setupContextProperties() override;

    /**
     * @brief Setup signal/slot connections
     */
    void setupConnections() override;

    /**
     * @brief Get main QML file path
     * @return QUrl to main.qml
     */
    QUrl getMainQmlPath() const override;

    /**
     * @brief Cleanup application components
     */
    void cleanup() override;

  private:
    // Application components
    ConfigManager* m_configManager;
    TranslationManager* m_translationManager;
    AppInfo* m_appInfo;
    WordGenerator* m_wordGenerator;
    TypingTest* m_typingTest;
    ThemeManager* m_themeManager;
    TestHistory* m_testHistory;
};

#endif  // XFINGERCRACKAPPLICATION_HPP
