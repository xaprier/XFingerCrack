import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    required property string title
    required property string value
    required property string icon
    
    Layout.fillWidth: true
    Layout.preferredHeight: 100
    color: themeManager.currentTheme.surface
    radius: 8
    border.width: 1
    border.color: themeManager.currentTheme.border
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 10
        
        Text {
            text: root.icon
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
        }
        
        Text {
            text: root.title
            font.pixelSize: 12
            color: themeManager.currentTheme.textSecondary
            Layout.alignment: Qt.AlignHCenter
        }
        
        Text {
            text: root.value
            font.pixelSize: 18
            font.bold: true
            color: themeManager.currentTheme.text
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
