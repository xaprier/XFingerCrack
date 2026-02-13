import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    required property string title
    required property int limit
    
    Layout.fillWidth: true
    Layout.preferredHeight: 150
    color: themeManager.currentTheme.surface
    radius: 12
    border.width: 1
    border.color: themeManager.currentTheme.border
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        Text {
            text: root.title
            font.pixelSize: 18
            font.bold: true
            color: themeManager.currentTheme.text
            Layout.alignment: Qt.AlignHCenter
        }
        
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            
            Row {
                anchors.centerIn: parent
                spacing: 60
                
                Column {
                    spacing: 5
                    
                    Text {
                        text: qsTr("Average WPM")
                        font.pixelSize: 12
                        color: themeManager.currentTheme.textSecondary
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    
                    Text {
                        text: {
                            var stats = testHistory.GetAverageStats(root.limit)
                            return stats && stats.avgWpm ? Math.round(stats.avgWpm).toString() : "0"
                        }
                        font.pixelSize: 28
                        font.bold: true
                        color: themeManager.currentTheme.accent
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                
                Column {
                    spacing: 5
                    
                    Text {
                        text: qsTr("Average Accuracy")
                        font.pixelSize: 12
                        color: themeManager.currentTheme.textSecondary
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    
                    Text {
                        text: {
                            var stats = testHistory.GetAverageStats(root.limit)
                            return stats && stats.avgAccuracy ? stats.avgAccuracy.toFixed(1) + "%" : "0%"
                        }
                        font.pixelSize: 28
                        font.bold: true
                        color: themeManager.currentTheme.correct
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }
    }
}
