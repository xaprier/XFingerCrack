import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root
    
    property var testResult: typingTest.testResult
    readonly property var stats: testResult && testResult.statistics ? testResult.statistics : null
    
    onVisibleChanged: {
        if (visible) {
            tryAgainButton.forceActiveFocus()
        }
    }
    
    Flickable {
        anchors.fill: parent
        contentHeight: contentColumn.height + 40
        clip: true
        focus: true
        
        // Keyboard shortcuts
        Keys.onPressed: (event) => {
            // Tab to focus Try Again button
            if (event.key === Qt.Key_Tab) {
                event.accepted = true
                tryAgainButton.forceActiveFocus()
            }
            // Enter/Return to restart test
            else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                event.accepted = true
                typingTest.ResetTest()
            }
        }
        
        Column {
            id: contentColumn
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(parent.width - 40, 1200)
            spacing: 30
            topPadding: 20
            bottomPadding: 20
            
            // Title Section
            Rectangle {
                width: parent.width
                height: 70
                color: themeManager.currentTheme.surface
                radius: 15
                
                RowLayout {
                    anchors.centerIn: parent
                    spacing: 20
                    
                    Column {
                        spacing: 3
                        Layout.preferredWidth: 250
                        TextWithFont {
                            text: qsTr("Test Complete!")
                            color: themeManager.currentTheme.accent
                            font.pixelSize: 22
                            font.bold: true
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }
                        TextWithFont {
                            text: (stats && stats.totalTime) ? (stats.totalTime / 1000).toFixed(1) + "s" : "0.0s"
                            color: themeManager.currentTheme.textSecondary
                            font.pixelSize: 12
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
            
            // Summary Statistics
            ResultSummary {
                width: parent.width
                stats: root.stats
            }
            
            // WPM Over Time Chart
            ResultWPMOverTime {
                width: parent.width
                testResult: root.testResult
            }
            
            // Typed Text Visualization (like monkeytype)
            ResultTypedText {
                width: parent.width
                testResult: root.testResult
            }
            
            // Actions
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 15
                
                TextWithFont {
                    text: qsTr("Press Tab+Enter restart")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 14
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 20
                    
                    Button {
                        id: tryAgainButton
                        text: qsTr("Try Again")
                        width: 150
                        height: 50
                        
                        Keys.onReturnPressed: {
                            typingTest.ResetTest()
                        }
                        
                        Keys.onEnterPressed: {
                            typingTest.ResetTest()
                        }
                        
                        onClicked: {
                            typingTest.ResetTest()
                            // The rest is handled by connections in main.qml and TypingArea
                        }
                        
                        background: Rectangle {
                            color: parent.pressed ? Qt.darker(themeManager.currentTheme.accent, 1.2) : 
                                   parent.hovered ? Qt.lighter(themeManager.currentTheme.accent, 1.1) : 
                                   themeManager.currentTheme.accent
                            radius: 8
                            border.width: parent.activeFocus ? 3 : 0
                            border.color: themeManager.currentTheme.text
                            
                            Behavior on border.width {
                                NumberAnimation { duration: 150 }
                            }
                            
                            scale: parent.activeFocus ? 1.05 : 1.0
                            
                            Behavior on scale {
                                NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
                            }
                        }
                        
                        contentItem: TextWithFont {
                            text: parent.text
                            color: themeManager.currentTheme.background
                            font.pixelSize: 16
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }
    }
}
