import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "pages"
import "components"

ApplicationWindow {
    id: root
    width: 1200
    height: 800
    minimumWidth: 1090
    minimumHeight: 915
    visible: true
    title: qsTr("%1 - Typing Test").arg(appInfo.applicationName)
    
    color: themeManager.currentTheme.background

    // Set global font for entire application from backend FontManager
    font.family: fontManager.currentFont
    
    // Watch for font changes from backend and force update
    Connections {
        target: fontManager
        function onCurrentFontChanged() {
            // Force update the font property
            root.font.family = fontManager.currentFont
        }
    }
    
    // Listen for language changes and trigger retranslation
    Connections {
        target: translationManager
        function onCurrentLanguageChanged() {
            console.log("[UI] Language changed to", translationManager.currentLanguage)
        }
    }
    
    // Global key event filter for ESC
    Shortcut {
        sequence: "Esc"
        enabled: !settingsPopup.opened
        onActivated: {
            if (showHistory) {
                showHistory = false
                typingArea.forceActiveFocus()
            } else if (showResults) {
                showResults = false
                typingArea.forceActiveFocus()
            } else {
                settingsPopup.open()
            }
        }
    }
    
    // Responsive configuration
    ResponsiveConfig {
        id: responsive
        Component.onCompleted: {
            updateSize(root.width, root.height)
        }
    }
    
    // Update responsive config when window size changes
    onWidthChanged: {
        responsive.updateSize(width, height)
    }
    onHeightChanged: {
        responsive.updateSize(width, height)
    }
    
    // Settings popup
    Settings {
        id: settingsPopup
    }
    
    // Keyboard event handling
    Item {
        id: keyHandler
        anchors.fill: parent
        focus: false
        
        Keys.onPressed: (event) => {
            // Always redirect to typing area if not active and valid key
            if (!typingTest.isActive && !showResults && !showHistory &&
                event.key !== Qt.Key_Tab && 
                event.key !== Qt.Key_Escape && 
                event.text.length > 0) {
                typingArea.forceActiveFocus()
                // Let the typing area handle this key
            }
        }
    }
    
    Column {
        anchors.fill: parent
        spacing: 0
        
        // Toolbar
        Toolbar {
            id: toolbar
            width: parent.width
        }
        
        // Main content area
        Item {
            width: parent.width
            height: parent.height - toolbar.height
            
            // Typing area (shown during test or before test)
            TypingArea {
                id: typingArea
                anchors.centerIn: parent
                width: Math.min(parent.width - 100, 1000)
                height: parent.height - 100
                visible: !showResults && !showHistory
                enabled: !showResults && !showHistory
                
                onVisibleChanged: {
                    if (visible) {
                        Qt.callLater(function() {
                            typingArea.forceActiveFocus()
                        })
                    }
                }
            }
            
            // Results screen (lazy loaded when needed)
            Loader {
                id: resultsLoader
                anchors.fill: parent
                active: showResults
                sourceComponent: ResultsScreen {
                    anchors.fill: parent
                }
                onLoaded: {
                }
                onStatusChanged: {
                    console.log("Loader status changed:", status)
                    if (status === Loader.Error) {
                        console.error("Loader error: Failed to load ResultsScreen")
                    }
                }
            }
            
            // History screen (lazy loaded when needed)
            Loader {
                id: historyLoader
                anchors.fill: parent
                active: showHistory
                sourceComponent: History {
                    anchors.fill: parent
                }
            }
        }
    }
    
    property bool showResults: false
    property bool showHistory: false
    
    // Reset test when leaving results screen
    onShowResultsChanged: {
        if (!showResults) {
            typingTest.ResetTest()
        }
    }
    
    // Show results screen when test completes
    Connections {
        target: typingTest
        function onTestCompleted() {
            showResults = true
        }
    }
    
    // Hide results screen when test is reset or user starts typing
    Connections {
        target: typingTest
        function onUserInputChanged() {
            if (typingTest.userInput === "" && !typingTest.isActive) {
                showResults = false
                typingArea.forceActiveFocus()
            }
        }
    }
    
    Component.onCompleted: {
        typingArea.forceActiveFocus()
    }
    
    // About popup
    About {
        id: aboutPopup
    }
}
