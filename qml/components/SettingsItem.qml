import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * @brief Reusable settings list item component
 * 
 * This component represents a single item in the settings list
 * with an icon, name, description, and arrow indicator.
 */
Rectangle {
    id: root
    
    // Properties
    property string icon: ""
    property string name: ""
    property string description: ""
    property string key: ""
    property bool isCurrentItem: false
    property int index: -1
    
    // Signals
    signal clicked()
    
    // Dimensions
    width: parent ? parent.width : 200
    height: 45
    radius: 4
    
    // Colors
    color: isCurrentItem ? themeManager.currentTheme.accent : 
           (mouseArea.containsMouse ? themeManager.currentTheme.surface : "transparent")
    
    // Content layout
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 10
        
        // Icon
        Item {
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30
            
            Text {
                anchors.centerIn: parent
                text: root.icon
                font.pixelSize: 18
            }
        }
        
        // Text content
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            
            Text {
                text: root.name
                font.pixelSize: 14
                font.bold: true
                color: root.isCurrentItem ? themeManager.currentTheme.background : themeManager.currentTheme.text
                Layout.fillWidth: true
            }
            
            Text {
                text: root.description
                font.pixelSize: 11
                color: root.isCurrentItem ? themeManager.currentTheme.background : themeManager.currentTheme.textSecondary
                opacity: root.isCurrentItem ? 0.9 : 0.8
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }
        
        // Arrow indicator
        Item {
            Layout.preferredWidth: 20
            Layout.preferredHeight: 30
            
            Text {
                anchors.centerIn: parent
                text: "›"
                font.pixelSize: 20
                color: root.isCurrentItem ? themeManager.currentTheme.background : themeManager.currentTheme.textSecondary
                opacity: 0.6
            }
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
