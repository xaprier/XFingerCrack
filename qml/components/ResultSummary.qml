import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    
    property var stats: ({})
    
    width: parent.width
    height: summaryFlow.implicitHeight + 30
    color: themeManager.currentTheme.surface
    radius: 15
    
    Flow {
        id: summaryFlow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 15
        width: Math.min(parent.width - 40, 1000)
        spacing: 20
        
        // WPM
        Column {
            width: Math.max(120, summaryFlow.width < 700 ? (summaryFlow.width - 40) / 2 : (summaryFlow.width - 100) / 6)
            spacing: 5
            Text {
                text: qsTr("WPM")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: (stats && stats.finalWpm !== undefined) ? stats.finalWpm : 0
                color: themeManager.currentTheme.accent
                font.pixelSize: 32
                font.bold: true
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Accuracy
        Column {
            width: Math.max(120, summaryFlow.width < 700 ? (summaryFlow.width - 40) / 2 : (summaryFlow.width - 100) / 6)
            spacing: 5
            Text {
                text: qsTr("Accuracy")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: (stats && stats.accuracy !== undefined) ? stats.accuracy.toFixed(1) + "%" : "0.0%"
                color: themeManager.currentTheme.text
                font.pixelSize: 32
                font.bold: true
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Correct Characters
        Column {
            width: Math.max(120, summaryFlow.width < 700 ? (summaryFlow.width - 40) / 2 : (summaryFlow.width - 100) / 6)
            spacing: 5
            Text {
                text: qsTr("Correct Keys")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: (stats && stats.correctCharacters !== undefined) ? stats.correctCharacters : 0
                color: themeManager.currentTheme.correct
                font.pixelSize: 32
                font.bold: true
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Incorrect Characters
        Column {
            width: Math.max(120, summaryFlow.width < 700 ? (summaryFlow.width - 40) / 2 : (summaryFlow.width - 100) / 6)
            spacing: 5
            Text {
                text: qsTr("Wrong Keys")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: (stats && stats.incorrectCharacters !== undefined) ? stats.incorrectCharacters : 0
                color: themeManager.currentTheme.incorrect
                font.pixelSize: 32
                font.bold: true
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Skipped Characters
        Column {
            width: Math.max(120, summaryFlow.width < 700 ? (summaryFlow.width - 40) / 2 : (summaryFlow.width - 100) / 6)
            spacing: 5
            Text {
                text: qsTr("Skipped Keys")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: (stats && stats.skippedCharacters !== undefined) ? stats.skippedCharacters : 0
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 32
                font.bold: true
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Corrected Characters
        Column {
            width: Math.max(120, summaryFlow.width < 700 ? (summaryFlow.width - 40) / 2 : (summaryFlow.width - 100) / 6)
            spacing: 5
            Text {
                text: qsTr("Corrected Keys")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: (stats && stats.correctedCharacters !== undefined) ? stats.correctedCharacters : 0
                color: themeManager.currentTheme.accent
                font.pixelSize: 32
                font.bold: true
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
