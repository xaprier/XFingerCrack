import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    
    property string dialogTitle: ""
    property string dialogText: ""
    property string confirmButtonText: qsTr("Confirm")
    property string cancelButtonText: qsTr("Cancel")
    property bool showCancelButton: true
    
    signal confirmed()
    signal cancelled()
    
    anchors.centerIn: Overlay.overlay
    width: Math.min(450, parent.width - 40)
    modal: true
    closePolicy: Popup.CloseOnEscape
    
    background: Rectangle {
        color: themeManager.currentTheme.surface
        radius: 12
        border.width: 2
        border.color: themeManager.currentTheme.border
    }
    
    header: Rectangle {
        height: 60
        color: "transparent"
        
        Text {
            anchors.centerIn: parent
            text: root.dialogTitle
            font.pixelSize: 20
            font.bold: true
            color: themeManager.currentTheme.text
        }
    }
    
    contentItem: Rectangle {
        implicitHeight: contentText.implicitHeight + 40
        color: "transparent"
        
        Text {
            id: contentText
            anchors.fill: parent
            anchors.margins: 20
            text: root.dialogText
            wrapMode: Text.WordWrap
            color: themeManager.currentTheme.text
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
    
    footer: Rectangle {
        height: 70
        color: "transparent"
        
        RowLayout {
            anchors.centerIn: parent
            spacing: 15
            
            Button {
                visible: root.showCancelButton
                text: root.cancelButtonText
                implicitWidth: 120
                implicitHeight: 40
                
                background: Rectangle {
                    color: parent.pressed ? Qt.darker(themeManager.currentTheme.surface, 1.2) : themeManager.currentTheme.surface
                    radius: 8
                    border.width: 1
                    border.color: themeManager.currentTheme.border
                }
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    color: themeManager.currentTheme.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    root.cancelled()
                    root.close()
                }
            }
            
            Button {
                text: root.confirmButtonText
                implicitWidth: 120
                implicitHeight: 40
                
                background: Rectangle {
                    color: parent.pressed ? Qt.darker(themeManager.currentTheme.accent, 1.2) : themeManager.currentTheme.accent
                    radius: 8
                    border.width: 0
                }
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    font.bold: true
                    color: themeManager.currentTheme.background
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    root.confirmed()
                    root.close()
                }
            }
        }
    }
}
