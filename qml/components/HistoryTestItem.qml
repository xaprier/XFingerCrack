import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property var testTimestamp: null
    property string mode: ""
    property int testDuration: 0
    property int words: 0
    property int wpmValue: 0
    property real accuracyValue: 0
    property int correct: 0
    property int incorrect: 0
    property int testId: 0
    property bool hovered: hoverArea.containsMouse

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }

    signal deleteClicked(int id)
    
    width: parent ? parent.width : 0
    implicitHeight: gridLayout.implicitHeight + 30
    color: root.hovered ? Qt.darker(themeManager.currentTheme.surface, 1.2) : themeManager.currentTheme.surface
    radius: 8
    border.width: 1
    border.color: root.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border

    Behavior on border.color {
        ColorAnimation { duration: 150 }
    }
    Behavior on color {
        ColorAnimation { duration: 150 }
    }
    
    GridLayout {
        id: gridLayout
        anchors.fill: parent
        anchors.margins: 15
        columns: Math.max(1, Math.floor(parent.width / 120))
        rowSpacing: 10
        columnSpacing: 15
        
        // Date & Time
        Column {
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            spacing: 3
            
            TextWithFont {
                text: root.testTimestamp ? new Date(root.testTimestamp).toLocaleDateString() : ""
                font.pixelSize: 11
                color: themeManager.currentTheme.textSecondary
            }
            TextWithFont {
                text: root.testTimestamp ? Qt.formatTime(new Date(root.testTimestamp), "HH:mm") : ""
                font.pixelSize: 16
                font.bold: true
                color: themeManager.currentTheme.text
            }
        }
        
        // Mode & Duration/Words
        Column {
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            spacing: 3
            
            TextWithFont {
                text: root.mode === "time" ? qsTr("Time") : qsTr("Words")
                font.pixelSize: 11
                color: themeManager.currentTheme.textSecondary
            }
            TextWithFont {
                text: root.mode === "time" 
                    ? root.testDuration + "s"
                    : root.words + " " + qsTr("words")
                font.pixelSize: 16
                font.bold: true
                color: themeManager.currentTheme.text
            }
        }
        
        // WPM
        Column {
            Layout.fillWidth: true
            Layout.minimumWidth: 80
            spacing: 3
            
            TextWithFont {
                text: qsTr("WPM")
                font.pixelSize: 11
                color: themeManager.currentTheme.textSecondary
            }
            TextWithFont {
                text: root.wpmValue ? root.wpmValue.toString() : "0"
                font.pixelSize: 20
                font.bold: true
                color: themeManager.currentTheme.accent
            }
        }
        
        // Accuracy
        Column {
            Layout.fillWidth: true
            Layout.minimumWidth: 90
            spacing: 3
            
            TextWithFont {
                text: qsTr("Accuracy")
                font.pixelSize: 11
                color: themeManager.currentTheme.textSecondary
            }
            TextWithFont {
                text: root.accuracyValue ? root.accuracyValue.toFixed(1) + "%" : "0%"
                font.pixelSize: 20
                font.bold: true
                color: root.accuracyValue >= 95 ? themeManager.currentTheme.correct : themeManager.currentTheme.text
            }
        }

        // Correct keys
        Column {
            Layout.fillWidth: true
            Layout.minimumWidth: 80
            spacing: 3
            
            TextWithFont {
                text: qsTr("Correct")
                font.pixelSize: 11
                color: themeManager.currentTheme.textSecondary
            }
            TextWithFont {
                text: root.correct ? root.correct.toString() : "0"
                font.pixelSize: 18
                font.bold: true
                color: themeManager.currentTheme.correct
            }
        }

        // Incorrect keys
        Column {
            Layout.fillWidth: true
            Layout.minimumWidth: 80
            spacing: 3

            TextWithFont {
                text: qsTr("Incorrect")
                font.pixelSize: 11
                color: themeManager.currentTheme.textSecondary
            }
            TextWithFont {
                text: root.incorrect ? root.incorrect.toString() : "0"
                font.pixelSize: 18
                font.bold: true
                color: themeManager.currentTheme.incorrect
            }
        }
        
        // Delete Button
        Button {
            Layout.fillWidth: true
            Layout.minimumWidth: 60
            Layout.preferredHeight: 40
            text: "🗑️"
            font.pixelSize: 18
            hoverEnabled: true
            
            onClicked: root.deleteClicked(root.testId)
            
            ThemedToolTip {
                visible: parent.hovered
                text: qsTr("Delete this test")
            }
            
            background: Rectangle {
                color: parent.pressed ? Qt.darker(themeManager.currentTheme.incorrect, 1.2) : themeManager.currentTheme.surface
                radius: 6
                border.width: 1
                border.color: parent.hovered ? themeManager.currentTheme.incorrect : themeManager.currentTheme.border

                Behavior on border.color {
                    ColorAnimation { duration: 150 }
                }
            }
            
            contentItem: TextWithFont {
                text: parent.text
                font: parent.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
