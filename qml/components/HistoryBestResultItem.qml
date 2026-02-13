import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property int rank: 0
    property int duration: 0
    property int wordCount: 0
    property int wpm: 0
    property real accuracy: 0.0
    property var timestamp: new Date()
    property bool isTimeBased: true
    
    implicitHeight: gridLayout.implicitHeight + 30
    color: themeManager.currentTheme.surface
    radius: 8
    border.width: 1
    border.color: themeManager.currentTheme.border
    
    GridLayout {
        id: gridLayout
        anchors.fill: parent
        anchors.margins: 15
        columns: 5
        rowSpacing: 8
        columnSpacing: 15
        
        // Rank
        Column {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            spacing: 3
            
            Text {
                text: qsTr("Rank")
                font.pixelSize: 10
                color: themeManager.currentTheme.textSecondary
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: "#" + root.rank
                font.pixelSize: 18
                font.bold: true
                color: themeManager.currentTheme.textSecondary
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Duration or Word Count
        Column {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            spacing: 3
            
            Text {
                text: root.isTimeBased ? qsTr("Duration") : qsTr("Words")
                font.pixelSize: 10
                color: themeManager.currentTheme.textSecondary
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: root.isTimeBased ? (root.duration + "s") : root.wordCount.toString()
                font.pixelSize: 14
                font.bold: true
                color: themeManager.currentTheme.text
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // WPM
        Column {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            spacing: 3
            
            Text {
                text: qsTr("WPM")
                font.pixelSize: 10
                color: themeManager.currentTheme.textSecondary
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: root.wpm.toString()
                font.pixelSize: 18
                font.bold: true
                color: themeManager.currentTheme.accent
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Accuracy
        Column {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            spacing: 3
            
            Text {
                text: qsTr("Accuracy")
                font.pixelSize: 10
                color: themeManager.currentTheme.textSecondary
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: root.accuracy.toFixed(1) + "%"
                font.pixelSize: 18
                font.bold: true
                color: themeManager.currentTheme.correct
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Date
        Column {
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            spacing: 3
            
            Text {
                text: qsTr("Date")
                font.pixelSize: 10
                color: themeManager.currentTheme.textSecondary
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                text: root.timestamp.toLocaleDateString()
                font.pixelSize: 12
                color: themeManager.currentTheme.textSecondary
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
