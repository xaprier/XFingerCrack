#include "AbstractQmlApplication.hpp"

#include <QDebug>

AbstractQmlApplication::AbstractQmlApplication(int& argc, char** argv)
    : QApplication(argc, argv), m_engine(new QQmlApplicationEngine(this)) {
}

AbstractQmlApplication::~AbstractQmlApplication() {
    // Clean up before destruction
    cleanup();

    // Clear the engine before QApplication destructor
    if (m_engine) {
        // Clear all root objects and connections
        m_engine->clearComponentCache();
        delete m_engine;
        m_engine = nullptr;
    }
}

int AbstractQmlApplication::run() {
    // Initialize components (create managers, load settings, etc.)
    initializeComponents();

    // Register custom QML types
    registerQmlTypes();

    // Setup context properties (expose C++ objects to QML)
    setupContextProperties();

    // Setup signal/slot connections
    setupConnections();

    // Get main QML file path
    QUrl mainQmlUrl = getMainQmlPath();

    // Connect to handle QML loading errors
    QObject::connect(
        m_engine, &QQmlApplicationEngine::objectCreated,
        this,
        [mainQmlUrl](QObject* obj, const QUrl& objUrl) {
            if (!obj && mainQmlUrl == objUrl) {
                qCritical() << "Failed to load QML file:" << mainQmlUrl;
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);

    // Load the main QML file
    m_engine->load(mainQmlUrl);

    // Check if QML loaded successfully
    if (m_engine->rootObjects().isEmpty()) {
        qCritical() << "No root objects in QML engine";
        return -1;
    }

    // Start the event loop
    return exec();
}
