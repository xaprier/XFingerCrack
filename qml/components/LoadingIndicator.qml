import QtQuick
import QtQuick.Controls

Item {
    id: root
    
    property bool loading: false
    property string text: qsTr("Loading...")
    property color indicatorColor: themeManager.currentTheme.accent
    property color textColor: themeManager.currentTheme.text
    
    visible: loading
    
    Rectangle {
        anchors.fill: parent
        color: themeManager.currentTheme.background
        opacity: 0.95
        
        Column {
            anchors.centerIn: parent
            spacing: 20
            
            // Spinning circle indicator
            Item {
                width: 60
                height: 60
                anchors.horizontalCenter: parent.horizontalCenter
                
                Rectangle {
                    id: spinner
                    width: 50
                    height: 50
                    radius: 25
                    anchors.centerIn: parent
                    color: "transparent"
                    border.width: 4
                    border.color: root.indicatorColor
                    opacity: 0.3
                }
                
                Rectangle {
                    width: 50
                    height: 50
                    radius: 25
                    anchors.centerIn: parent
                    color: "transparent"
                    border.width: 4
                    border.color: root.indicatorColor
                    
                    // Create arc effect
                    rotation: 0
                    
                    Rectangle {
                        width: parent.width
                        height: parent.height / 2
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: themeManager.currentTheme.background
                    }
                    
                    RotationAnimation on rotation {
                        running: root.loading
                        loops: Animation.Infinite
                        from: 0
                        to: 360
                        duration: 1000
                        easing.type: Easing.Linear
                    }
                }
            }
            
            TextWithFont {
                text: root.text
                font.pixelSize: 16
                color: root.textColor
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
