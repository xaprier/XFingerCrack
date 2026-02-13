import QtQuick
import QtQuick.Controls

ToolTip {
    id: root
    
    delay: 500
    timeout: 3000
    
    background: Rectangle {
        color: themeManager.currentTheme.surface
        border.color: themeManager.currentTheme.border
        border.width: 1
        radius: 6
    }
    
    contentItem: Text {
        text: root.text
        font.pixelSize: 12
        color: themeManager.currentTheme.text
    }
    
    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 150 }
    }
    
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 100 }
    }
}
