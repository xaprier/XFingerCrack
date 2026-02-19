#include "AppInfo.hpp"

#include <QVersionNumber>

#include "Version.hpp"

AppInfo::AppInfo(QObject* parent) : QObject(parent) {}

QString AppInfo::ApplicationName() const { return QString(APP_NAME); }

QString AppInfo::Version() const { return QString(APP_VERSION); }

QString AppInfo::QtVersion() const { return QString(QT_VERSION_STR); }

QString AppInfo::Organization() const { return QString(APP_ORGANIZATION); }

QString AppInfo::GithubUrl() const { return QString(APP_GITHUB_URL); }
