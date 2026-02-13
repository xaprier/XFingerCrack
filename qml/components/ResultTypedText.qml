import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    
    property var testResult
    
    width: parent.width
    height: Math.max(150, typedTextFlow.implicitHeight + 80)
    color: themeManager.currentTheme.surface
    radius: 15
    
    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 5
        
        Text {
            text: qsTr("Typed Text")
            color: themeManager.currentTheme.text
            font.pixelSize: 18
            font.bold: true
        }
        
        // Scrollable text area with character-by-character coloring
        Flickable {
            width: parent.width
            height: Math.min(200, typedTextFlow.implicitHeight + 40)
            contentHeight: typedTextFlow.implicitHeight + 20
            clip: true
            
            Flow {
                id: typedTextFlow
                width: parent.width
                anchors.top: parent.top
                spacing: 0
                
                Repeater {
                    id: typedTextRepeater
                    model: testResult ? testResult.typedTextHistory : []
                    

                    
                    MouseArea {
                        width: charDisplay.width
                        height: charDisplay.height
                        hoverEnabled: true
                        
                        ToolTip {
                            id: charTooltip
                            visible: parent.containsMouse && modelData.expectedWord !== undefined
                            text: {
                                var tooltip = "Expected: '" + (modelData.expectedWord || "") + "' | WPM: " + (modelData.wordWpm || 0);
                                if (modelData.correctionCount > 0) {
                                    tooltip += " | Corrections: " + modelData.correctionCount;
                                }
                                return tooltip;
                            }
                            delay: 200
                            
                            background: Rectangle {
                                color: themeManager.currentTheme.surface
                                border.color: themeManager.currentTheme.border
                                border.width: 1
                                radius: 4
                            }
                            
                            contentItem: Text {
                                text: charTooltip.text
                                color: themeManager.currentTheme.text
                                font.pixelSize: 12
                            }
                        }
                        
                        Column {
                            spacing: 0
                            
                            // Correction dots area (fixed height for all characters)
                            Item {
                                width: charDisplay.width
                                height: 8  // Fixed height for alignment
                                
                                Row {
                                    spacing: 2
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 2
                                    // Only show corrections for actually typed characters (not skipped)
                                    visible: modelData.correctionCount > 0 && modelData.finalState !== "skipped"
                                    
                                    Repeater {
                                        model: Math.min(modelData.correctionCount, 3) // Max 3 dots
                                        
                                        Rectangle {
                                            width: 4
                                            height: 4
                                            radius: 2
                                            color: modelData.finalState === "corrected" ? 
                                                themeManager.currentTheme.accent : 
                                                themeManager.currentTheme.textSecondary
                                        }
                                    }
                                }
                            }
                            
                            Text {
                                id: charDisplay
                                text: modelData.actualTyped === " " ? "␣" : modelData.actualTyped
                                font.pixelSize: 20
                                font.family: "Monospace"
                                
                                // Color based on state - show what user typed
                                color: {
                                    switch(modelData.finalState) {
                                        case "correct":
                                            return themeManager.currentTheme.correct
                                        case "corrected":
                                            return themeManager.currentTheme.accent
                                        case "incorrect":
                                            return themeManager.currentTheme.incorrect
                                        case "skipped":
                                            return themeManager.currentTheme.textSecondary
                                        default:
                                            return themeManager.currentTheme.text
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Legend
        Flow {
            width: parent.width
            spacing: 15
            
            Row {
                spacing: 5
                height: 20
                
                Rectangle {
                    width: 12
                    height: 12
                    radius: 2
                    color: themeManager.currentTheme.correct
                    y: (parent.height - height) / 2
                }
                Text {
                    text: qsTr("Correct")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 12
                    y: (parent.height - height) / 2
                }
            }
            
            Row {
                spacing: 5
                height: 20
                
                Rectangle {
                    width: 12
                    height: 12
                    radius: 2
                    color: themeManager.currentTheme.accent
                    y: (parent.height - height) / 2
                }
                
                Row {
                    spacing: 2
                    y: (parent.height - height) / 2
                    
                    Repeater {
                        model: 3
                        Rectangle {
                            width: 4
                            height: 4
                            radius: 2
                            color: themeManager.currentTheme.accent
                            y: -6
                        }
                    }
                }
                
                Text {
                    text: qsTr("Corrected")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 12
                    y: (parent.height - height) / 2
                }
            }
            
            Row {
                spacing: 5
                height: 20
                
                Rectangle {
                    width: 12
                    height: 12
                    radius: 2
                    color: themeManager.currentTheme.incorrect
                    y: (parent.height - height) / 2
                }
                Text {
                    text: qsTr("Incorrect")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 12
                    y: (parent.height - height) / 2
                }
            }
            
            Row {
                spacing: 5
                height: 20
                
                Rectangle {
                    width: 12
                    height: 12
                    radius: 2
                    color: themeManager.currentTheme.textSecondary
                    y: (parent.height - height) / 2
                }
                Text {
                    text: qsTr("Skipped")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 12
                    y: (parent.height - height) / 2
                }
            }
            
            Row {
                spacing: 5
                height: 20
                
                Text {
                    text: qsTr("(Hover for word info)")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 11
                    font.italic: true
                    y: (parent.height - height) / 2
                }
            }
        }
    }
}
