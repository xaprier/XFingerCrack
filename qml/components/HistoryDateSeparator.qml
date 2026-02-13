import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property string dateText: ""
    property int dailyAvgWpm: 0
    property real dailyAvgAcc: 0
    
    width: parent ? parent.width : 0
    height: 50
    color: "transparent"
    
    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 1
        color: themeManager.currentTheme.border
    }
    
    RowLayout {
        anchors.centerIn: parent
        spacing: 15
        
        Rectangle {
            implicitWidth: dateLabel.implicitWidth + 20
            implicitHeight: 30
            color: themeManager.currentTheme.background
            radius: 6
            border.width: 1
            border.color: themeManager.currentTheme.border
            
            Text {
                id: dateLabel
                anchors.centerIn: parent
                text: root.dateText
                font.pixelSize: 13
                font.bold: true
                color: themeManager.currentTheme.text
            }
        }
        
        Rectangle {
            visible: root.dailyAvgWpm > 0
            implicitWidth: avgText.implicitWidth + 20
            implicitHeight: 30
            color: themeManager.currentTheme.surface
            radius: 6
            border.width: 1
            border.color: themeManager.currentTheme.border
            
            Text {
                id: avgText
                anchors.centerIn: parent
                text: root.dailyAvgWpm > 0 
                    ? qsTr("Avg: ") + root.dailyAvgWpm + " WPM · " + root.dailyAvgAcc.toFixed(1) + "%"
                    : ""
                font.pixelSize: 11
                color: themeManager.currentTheme.textSecondary
            }
        }
    }
}
