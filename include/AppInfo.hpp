#ifndef APPINFO_HPP
#define APPINFO_HPP

#include <QObject>
#include <QQmlEngine>
#include <QString>

/**
 * @brief Application information provider
 *
 * This class provides read-only access to application metadata
 * such as name, version, Qt version, and relevant URLs.
 * Exposed to QML for display in About dialogs and similar UI elements.
 */
class AppInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ ApplicationName CONSTANT)
    Q_PROPERTY(QString version READ Version CONSTANT)
    Q_PROPERTY(QString qtVersion READ QtVersion CONSTANT)
    Q_PROPERTY(QString organization READ Organization CONSTANT)
    Q_PROPERTY(QString githubUrl READ GithubUrl CONSTANT)

  public:
    /**
     * @brief Constructor
     * @param parent Parent QObject
     */
    explicit AppInfo(QObject* parent = nullptr);

    /**
     * @brief Get application name
     * @return Application name string
     */
    QString ApplicationName() const;

    /**
     * @brief Get application version
     * @return Version string
     */
    QString Version() const;

    /**
     * @brief Get Qt version used to build the application
     * @return Qt version string
     */
    QString QtVersion() const;

    /**
     * @brief Get organization name
     * @return Organization name string
     */
    QString Organization() const;

    /**
     * @brief Get GitHub repository URL
     * @return GitHub URL string
     */
    QString GithubUrl() const;
};

#endif  // APPINFO_HPP
