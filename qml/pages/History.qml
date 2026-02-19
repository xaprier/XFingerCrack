import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root
    
    property bool loading: false
    
    Component.onCompleted: {
        loadStatistics()
    }
    
    function loadStatistics() {
        loading = true
        Qt.callLater(function() {
            loading = false
        })
    }
    
    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: Math.max(height, mainColumn.implicitHeight)
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick
        
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }
        
        ColumnLayout {
            id: mainColumn
            width: parent.width
            height: Math.max(parent.height, implicitHeight)
            spacing: 20
            
            // Header
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 70
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20
                    
                    Button {
                        text: "«"
                        font.pixelSize: 14
                        font.bold: true
                        padding: 10
                        implicitHeight: 40
                        onClicked: showHistory = false
                        hoverEnabled: true
                        
                        ThemedToolTip {
                            visible: parent.hovered
                            text: qsTr("Back to Test") + " (Esc)"
                        }
                        
                        background: Rectangle {
                            color: parent.pressed ? Qt.darker(themeManager.currentTheme.surface, 1.2) : themeManager.currentTheme.surface
                            radius: 8
                            border.width: 1
                            border.color: parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border
                            
                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: TextWithFont {
                            text: parent.text
                            font: parent.font
                            color: themeManager.currentTheme.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    
                    TextWithFont {
                        text: qsTr("Test History")
                        font.pixelSize: 32
                        font.bold: true
                        color: themeManager.currentTheme.text
                        Layout.fillWidth: true
                    }
                    
                    Button {
                        text: qsTr("Reset")
                        font.pixelSize: 14
                        font.bold: true
                        padding: 10
                        onClicked: clearDialog.open()
                        hoverEnabled: true
                        
                        ThemedToolTip {
                            visible: parent.hovered
                            text: qsTr("Delete all test history")
                        }
                        
                        background: Rectangle {
                            color: parent.pressed ? Qt.darker(themeManager.currentTheme.incorrect, 1.2) : themeManager.currentTheme.surface
                            radius: 8
                            border.width: 1
                            border.color: parent.hovered ? themeManager.currentTheme.incorrect : themeManager.currentTheme.border

                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: TextWithFont {
                            text: parent.text
                            font: parent.font
                            color: themeManager.currentTheme.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
            
            // Overview Stats
            HistorySummaryStats {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
            }
            
            // Tab Bar
            TabBar {
                id: tabBar
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.topMargin: -5
                
                background: Rectangle {
                    color: "transparent"
                }
                
                TabButton {
                    text: qsTr("Recent Tests")
                    hoverEnabled: true
                    background: Rectangle {
                        color: parent.checked ? themeManager.currentTheme.surface : "transparent"
                        radius: 8
                        border.width: 1
                        border.color: parent.checked ? themeManager.currentTheme.border : (parent.hovered ? themeManager.currentTheme.accent : "transparent")

                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    contentItem: TextWithFont {
                        text: parent.text
                        font.pixelSize: 14
                        color: themeManager.currentTheme.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                TabButton {
                    text: qsTr("Statistics")
                    hoverEnabled: true
                    background: Rectangle {
                        color: parent.checked ? themeManager.currentTheme.surface : "transparent"
                        radius: 8
                        border.width: 1
                        border.color: parent.checked ? themeManager.currentTheme.border : (parent.hovered ? themeManager.currentTheme.accent : "transparent")

                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    contentItem: TextWithFont {
                        text: parent.text
                        font.pixelSize: 14
                        color: themeManager.currentTheme.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                TabButton {
                    text: qsTr("Best Results")
                    hoverEnabled: true
                    background: Rectangle {
                        color: parent.checked ? themeManager.currentTheme.surface : "transparent"
                        radius: 8
                        border.width: 1
                        border.color: parent.checked ? themeManager.currentTheme.border : (parent.hovered ? themeManager.currentTheme.accent : "transparent")

                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    contentItem: TextWithFont {
                        text: parent.text
                        font.pixelSize: 14
                        color: themeManager.currentTheme.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
            
            // Content Stack
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 400
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.bottomMargin: 20
                currentIndex: tabBar.currentIndex
                
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: themeManager.currentTheme.surface
                    radius: 12
                    border.width: 2
                    border.color: themeManager.currentTheme.border
                    
                    HistoryRecentTests {
                        id: recentTestsView
                        anchors.fill: parent
                        anchors.margins: 20
                    }
                }
                
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: themeManager.currentTheme.background
                    
                    HistoryStatistics {
                        id: statisticsView
                        anchors.fill: parent
                        anchors.margins: 20
                    }
                }
                
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: themeManager.currentTheme.surface
                    radius: 12
                    border.width: 2
                    border.color: themeManager.currentTheme.border
                    
                    HistoryBestResults {
                        id: bestResultsView
                        anchors.fill: parent
                        anchors.margins: 20
                    }
                }
            }
        }
    } // Flickable
    
    // Loading Indicator
    LoadingIndicator {
        anchors.fill: parent
        loading: root.loading
    }
    
    // Clear Confirmation Dialog
    ModalDialog {
        id: clearDialog
        dialogTitle: qsTr("Clear All History")
        dialogText: qsTr("Are you sure you want to delete all test history? This action cannot be undone.")
        confirmButtonText: qsTr("Yes, Delete All")
        cancelButtonText: qsTr("Cancel")
        
        onConfirmed: {
            testHistory.ClearHistory()
            recentTestsView.refresh()
            statisticsView.refresh()
            bestResultsView.refresh()
        }
    }
}
