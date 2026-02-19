import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * @brief Reusable settings option item component
 * 
 * This component represents a single option in settings options list
 * with selection indicator.
 */
Rectangle {
    id: root
    
    // Properties
    property string label: ""
    property var value
    property bool isSelected: false
    property bool isCurrentItem: false
    
    // Signals
    signal clicked()
    
    // Dimensions
    width: parent ? parent.width : 200
    height: 40
    radius: 4
    
    // Colors
    color: isSelected ? Qt.rgba(themeManager.currentTheme.accent.r, 
                                 themeManager.currentTheme.accent.g, 
                                 themeManager.currentTheme.accent.b, 0.2) : 
           (mouseArea.containsMouse || isCurrentItem ? Qt.lighter(themeManager.currentTheme.surface, 1.1) : 
                                                       themeManager.currentTheme.surface)
    
    border.color: isSelected ? themeManager.currentTheme.accent : themeManager.currentTheme.border
    border.width: isSelected ? 2 : 1
    
    // Content layout
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 10
        
        TextWithFont {
            text: root.label
            font.pixelSize: 13
            color: themeManager.currentTheme.text
            Layout.fillWidth: true
        }
        
        TextWithFont {
            text: root.isSelected ? "✓" : ""
            font.pixelSize: 16
            color: themeManager.currentTheme.accent
        }
    }
    
    // Mouse interaction
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        
        onClicked: root.clicked()
    }
}
