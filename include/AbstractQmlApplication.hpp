#ifndef ABSTRACTQMLAPPLICATION_HPP
#define ABSTRACTQMLAPPLICATION_HPP

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

/**
 * @brief Abstract base class for QML-based applications
 *
 * This class provides the foundational infrastructure for applications
 * that use QML for their user interface. It handles QML engine setup,
 * context property management, and application lifecycle.
 */
class AbstractQmlApplication : public QApplication {
    Q_OBJECT

  public:
    /**
     * @brief Constructor
     * @param argc Argument count
     * @param argv Argument values
     */
    AbstractQmlApplication(int& argc, char** argv);

    /**
     * @brief Destructor
     */
    virtual ~AbstractQmlApplication();

    /**
     * @brief Initialize and run the application
     * @return Exit code
     */
    int run();

  protected:
    /**
     * @brief Get the QML engine instance
     * @return Pointer to QQmlApplicationEngine
     */
    QQmlApplicationEngine* engine() const { return m_engine; }

    /**
     * @brief Get the root context for setting context properties
     * @return Pointer to QQmlContext
     */
    QQmlContext* rootContext() const { return m_engine->rootContext(); }

    /**
     * @brief Pure virtual: Setup application-specific context properties
     *
     * Override this method to expose C++ objects to QML by calling
     * rootContext()->setContextProperty()
     */
    virtual void setupContextProperties() = 0;

    /**
     * @brief Pure virtual: Setup application-specific signal/slot connections
     *
     * Override this method to establish connections between objects
     */
    virtual void setupConnections() = 0;

    /**
     * @brief Pure virtual: Get the main QML file path
     * @return QUrl pointing to the main QML file
     */
    virtual QUrl getMainQmlPath() const = 0;

    /**
     * @brief Virtual: Initialize application components before QML loading
     *
     * Override this method to create and initialize objects, managers, etc.
     * Default implementation does nothing.
     */
    virtual void initializeComponents() {}

    /**
     * @brief Virtual: Register custom QML types
     *
     * Override this method to call qmlRegisterType, qmlRegisterUncreatableType, etc.
     * Default implementation does nothing.
     */
    virtual void registerQmlTypes() {}

    /**
     * @brief Virtual: Perform cleanup before application exit
     *
     * Override this method to perform custom cleanup operations.
     * Default implementation does nothing.
     */
    virtual void cleanup() {}

  private:
    QQmlApplicationEngine* m_engine;
};

#endif  // ABSTRACTQMLAPPLICATION_HPP
