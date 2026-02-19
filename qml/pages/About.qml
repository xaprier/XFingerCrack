import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Popup {
    id: aboutPopup
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    
    anchors.centerIn: Overlay.overlay
    width: Math.min(500, parent.width - 40)
    height: Math.min(600, parent.height - 40)
    
    background: Rectangle {
        color: themeManager.currentTheme.background
        border.color: themeManager.currentTheme.border
        border.width: 2
        radius: 8
    }
    
    onClosed: {
        settingsPopup.visible = true
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20
        
        // Header with close button
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            TextWithFont {
                text: qsTr("About")
                font.pixelSize: 24
                font.bold: true
                color: themeManager.currentTheme.text
                Layout.fillWidth: true
            }
            
            Rectangle {
                width: 32
                height: 32
                radius: 16
                color: themeManager.currentTheme.surface
                border.color: closeMouse.containsMouse ? themeManager.currentTheme.incorrect : themeManager.currentTheme.border
                border.width: 1
                
                TextWithFont {
                    anchors.centerIn: parent
                    text: "×"
                    font.pixelSize: 20
                    color: themeManager.currentTheme.text
                }

                Behavior on border.color {
                    ColorAnimation { duration: 150 }
                }
                
                MouseArea {
                    id: closeMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: aboutPopup.close()
                }
            }
        }
        
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: themeManager.currentTheme.border
        }
        
        // Logo section
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 180
            Layout.margins: 10
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 5
                
                Image {
                    id: logoImage
                    source: "qrc:/XFingerCrack.png"
                    Layout.preferredHeight: 180
                    Layout.preferredWidth: 180
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignHCenter
                    visible: status === Image.Ready
                    smooth: true
                }
                
                TextWithFont {
                    text: appInfo.applicationName
                    font.pixelSize: 28
                    font.bold: true
                    color: themeManager.currentTheme.accent
                    Layout.alignment: Qt.AlignHCenter
                    visible: logoImage.status !== Image.Ready
                }
                
                TextWithFont {
                    text: qsTr("Version") + " " + appInfo.version
                    font.pixelSize: 14
                    color: themeManager.currentTheme.textSecondary
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
        
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: themeManager.currentTheme.border
        }
        
        // Description
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            contentWidth: availableWidth
            
            ColumnLayout {
                width: parent.width
                spacing: 20
                
                // About the app
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    
                    TextWithFont {
                        text: qsTr("About the Application")
                        font.pixelSize: 16
                        font.bold: true
                        color: themeManager.currentTheme.text
                    }
                    
                    TextWithFont {
                        text: qsTr("%1 is a modern, feature-rich typing speed test application built with Qt. It helps you improve your typing skills by providing accurate WPM (Words Per Minute) measurements, comprehensive statistics, and a customizable interface.").arg(appInfo.applicationName)
                        font.pixelSize: 13
                        color: themeManager.currentTheme.textSecondary
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
                
                // Features
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    
                    TextWithFont {
                        text: qsTr("Features")
                        font.pixelSize: 16
                        font.bold: true
                        color: themeManager.currentTheme.text
                    }
                    
                    Column {
                        Layout.fillWidth: true
                        spacing: 4
                        
                        Repeater {
                            model: [
                                qsTr("⏱️ Time-based and word-count based testing modes"),
                                qsTr("🌍 Multi-language support (6 languages)"),
                                qsTr("🎨 Multiple beautiful themes"),
                                qsTr("🔊 Customizable keyboard sound effects"),
                                qsTr("📊 Real-time WPM and accuracy tracking"),
                                qsTr("🌐 Multi-language UI (Turkish, English, German)"),
                                qsTr("⚙️ Comprehensive settings with quick access")
                            ]
                            
                            TextWithFont {
                                text: modelData
                                font.pixelSize: 12
                                color: themeManager.currentTheme.textSecondary
                                wrapMode: Text.WordWrap
                                width: parent.width
                            }
                        }
                    }
                }
                
                // Developer
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    
                    TextWithFont {
                        text: qsTr("Developer")
                        font.pixelSize: 16
                        font.bold: true
                        color: themeManager.currentTheme.text
                    }
                    
                    TextWithFont {
                        text: qsTr("Developed with ❤️ by xaprier")
                        font.pixelSize: 13
                        color: themeManager.currentTheme.textSecondary
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        
                        Rectangle {
                            width: 100
                            height: 32
                            radius: 6
                            color: githubMouse.containsMouse ? Qt.lighter(themeManager.currentTheme.surface, 1.2) : themeManager.currentTheme.surface
                            border.color: themeManager.currentTheme.border
                            border.width: 1
                            
                            TextWithFont {
                                anchors.centerIn: parent
                                text: "GitHub"
                                font.pixelSize: 12
                                color: themeManager.currentTheme.text
                            }
                            
                            MouseArea {
                                id: githubMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: Qt.openUrlExternally(appInfo.githubUrl)
                            }
                        }
                    }
                }
                
                // Technology
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    
                    TextWithFont {
                        text: qsTr("Built With")
                        font.pixelSize: 16
                        font.bold: true
                        color: themeManager.currentTheme.text
                    }
                    
                    TextWithFont {
                        text: "Qt " + appInfo.qtVersion + " • QML • C++17 • CMake"
                        font.pixelSize: 13
                        color: themeManager.currentTheme.textSecondary
                    }
                }
            }
        }
        
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: themeManager.currentTheme.border
        }
        
        // Footer
        TextWithFont {
            text: qsTr("Press ESC to close")
            font.pixelSize: 11
            color: themeManager.currentTheme.textSecondary
            opacity: 0.7
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
