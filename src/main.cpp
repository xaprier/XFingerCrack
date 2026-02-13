#include <QDir>
#include <QDirIterator>

#include "XFingerCrackApplication.hpp"

void listAllQrcFiles() {
    QDirIterator it(":/", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        qDebug() << "QRC File:" << it.next();
    }
}

int main(int argc, char* argv[]) {
    // listAllQrcFiles();  // Debug: List all files in the resource system
    XFingerCrackApplication app(argc, argv);
    return app.run();
}
