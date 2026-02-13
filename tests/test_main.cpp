#include <gtest/gtest.h>

#include <QCoreApplication>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    // Set test organization and application name
    QCoreApplication::setOrganizationName("xaprier-test");
    QCoreApplication::setApplicationName("FingerCrack-test");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
