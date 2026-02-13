cmake_minimum_required(VERSION 3.16)

project(
    XFingerCrack 
    VERSION 1.0.0 
    LANGUAGES CXX
    DESCRIPTION "A local typing test application to measure and improve your typing speed and accuracy built with Qt6."
    HOMEPAGE_URL "https://github.com/xaprier/XFingerCrack"
)

set(PROJECT_ORGANIZATION "xaprier")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Disable QML cache generation to avoid qmlcachegen issues
set(QT_QML_GENERATE_QMLCACHE OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Charts Widgets Multimedia LinguistTools Sql Test)

if(COMMAND qt_standard_project_setup)
    qt_standard_project_setup()
endif()

file(GLOB_RECURSE HPP_FILES include/*.hpp)
file(GLOB_RECURSE CPP_FILES src/*.cpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Configure version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp
    @ONLY
)

set(PROJECT_SOURCES
    ${HPP_FILES}
    ${CPP_FILES}
)

# HANDLE TRANSLATION FILES
set(LANGS tr_TR en_US de_DE)
set(QM_FILES "")
foreach(LANG ${LANGS})
    qt_create_translation(QM_FILE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/resources/translations/translation_${LANG}.ts)
    list(APPEND QM_FILES ${QM_FILE})
endforeach()

foreach(QM_FILE ${QM_FILES})
    if(EXISTS ${QM_FILE})
        configure_file(${QM_FILE} ${CMAKE_SOURCE_DIR}/resources/translations COPYONLY)
    else()
        list(APPEND PROJECT_SOURCES ${TS_FILES} ${QM_FILES})
    endif()
endforeach()

qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    resources/resources.qrc
)

# Set Qt policies to suppress warnings
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0002 NEW)
qt_policy(SET QTP0003 NEW)
qt_policy(SET QTP0004 NEW)

# Collect QML files with relative paths
file(GLOB QML_FILES_ROOT qml/*.qml)
file(GLOB_RECURSE QML_FILES_SUBDIRS qml/**/*.qml)
set(QML_FILES_ABSOLUTE ${QML_FILES_ROOT} ${QML_FILES_SUBDIRS})
set(QML_FILES "")
foreach(QML_FILE ${QML_FILES_ABSOLUTE})
    file(RELATIVE_PATH QML_FILE_REL ${CMAKE_CURRENT_SOURCE_DIR} ${QML_FILE})
    list(APPEND QML_FILES ${QML_FILE_REL})
endforeach()

qt_add_qml_module(${PROJECT_NAME}
    URI XFingerCrack
    VERSION 1.0
    QML_FILES
        ${QML_FILES}
    NO_CACHEGEN
    NO_LINT
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::Charts
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Sql
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "fingercrack"
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Testing
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    
    # Fetch GoogleTest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Test executable
    add_executable(fingercrack_tests
        tests/test_main.cpp
        tests/test_testhistory.cpp
        tests/test_wordgenerator.cpp
        tests/test_typingtest.cpp
        tests/test_configmanager.cpp
        # Source files needed for testing
        src/TestHistory.cpp
        include/TestHistory.hpp
        src/WordGenerator.cpp
        include/WordGenerator.hpp
        src/TypingTest.cpp
        include/TypingTest.hpp
        src/ConfigManager.cpp
        include/ConfigManager.hpp
        src/TestResult.cpp
        include/TestResult.hpp
        # Resources for testing (wordlists, etc.)
        resources/resources.qrc
    )
    
    target_link_libraries(fingercrack_tests PRIVATE
        GTest::gtest_main
        Qt6::Core
        Qt6::Sql
        Qt6::Test
    )
    
    target_include_directories(fingercrack_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    include(GoogleTest)
    gtest_discover_tests(fingercrack_tests)
    
    # Test database population tool
    add_executable(populate_testdb
        tests/populate_testdb.cpp
        src/TestHistory.cpp
        include/TestHistory.hpp
    )
    
    target_link_libraries(populate_testdb PRIVATE
        Qt6::Core
        Qt6::Sql
    )
    
    target_include_directories(populate_testdb PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)