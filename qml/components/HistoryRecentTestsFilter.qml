import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property string selectedModeFilter: "all"
    property int selectedDurationFilter: 0
    
    signal filtersChanged()
    
    implicitHeight: contentRow.implicitHeight + 30
    color: "transparent"
    radius: 8
    border.width: 1
    border.color: themeManager.currentTheme.border
    
    RowLayout {
        id: contentRow
        anchors.fill: parent
        anchors.margins: 15
        spacing: 20
        
        Item { 
            Layout.fillWidth: true 
            visible: root.width >= 600
        }
        
        // Mode Filter
        Column {
            Layout.minimumWidth: root.width < 600 ? root.width - 30 : 240
            Layout.maximumWidth: root.width < 600 ? root.width - 30 : -1
            spacing: 8
            
            Text {
                text: qsTr("Test Mode")
                font.pixelSize: 12
                font.bold: true
                color: themeManager.currentTheme.text
            }
            
            RowLayout {
                width: parent.width
                spacing: 8
                
                Button {
                    id: allBtn
                    Layout.fillWidth: true
                    text: qsTr("All")
                    checkable: true
                    checked: root.selectedModeFilter === "all"
                    hoverEnabled: true
                    
                    onClicked: {
                        if (root.selectedModeFilter !== "all") {
                            root.selectedModeFilter = "all"
                            root.filtersChanged()
                        }
                    }
                    
                    background: Rectangle {
                        color: parent.checked ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                        radius: 6
                        border.width: 1
                        border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                        
                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 13
                        color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                Button {
                    id: timeBtn
                    Layout.fillWidth: true
                    text: qsTr("Time")
                    checkable: true
                    checked: root.selectedModeFilter === "time"
                    hoverEnabled: true
                    
                    onClicked: {
                        if (root.selectedModeFilter !== "time") {
                            root.selectedModeFilter = "time"
                            root.filtersChanged()
                        }
                    }
                    
                    background: Rectangle {
                        color: parent.checked ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                        radius: 6
                        border.width: 1
                        border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                        
                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 13
                        color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                Button {
                    id: wordsBtn
                    Layout.fillWidth: true
                    text: qsTr("Words")
                    checkable: true
                    checked: root.selectedModeFilter === "words"
                    hoverEnabled: true
                    
                    onClicked: {
                        if (root.selectedModeFilter !== "words") {
                            root.selectedModeFilter = "words"
                            root.filtersChanged()
                        }
                    }
                    
                    background: Rectangle {
                        color: parent.checked ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                        radius: 6
                        border.width: 1
                        border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                        
                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 13
                        color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
        
        // Duration/WordCount Filter
        Column {
            Layout.minimumWidth: root.width < 600 ? root.width - 30 : 500
            Layout.maximumWidth: root.width < 600 ? root.width - 30 : -1
            spacing: 8
            
            Text {
                text: root.selectedModeFilter === "time" ? qsTr("Duration") : root.selectedModeFilter === "words" ? qsTr("Word Count") : qsTr("Test Length")
                font.pixelSize: 12
                font.bold: true
                color: themeManager.currentTheme.text
            }
            
            Flow {
                width: parent.width
                spacing: 5
                
                // All option
                Button {
                    text: qsTr("All")
                    checkable: true
                    checked: root.selectedDurationFilter === 0
                    hoverEnabled: true
                    
                    onClicked: {
                        root.selectedDurationFilter = 0
                        root.filtersChanged()
                    }
                    
                    background: Rectangle {
                        implicitWidth: 45
                        implicitHeight: 30
                        color: parent.checked ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                        radius: 6
                        border.width: 1
                        border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                        
                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 11
                        color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                // Time-based options (shown when mode is "all" or "time")
                Repeater {
                    model: (root.selectedModeFilter === "all" || root.selectedModeFilter === "time") ? [15, 30, 60, 120] : []
                    
                    Button {
                        text: modelData + "s"
                        checkable: true
                        checked: root.selectedDurationFilter === modelData
                        hoverEnabled: true
                        
                        onClicked: {
                            root.selectedDurationFilter = modelData
                            root.filtersChanged()
                        }
                        
                        background: Rectangle {
                            implicitWidth: 45
                            implicitHeight: 30
                            color: parent.checked ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                            radius: 6
                            border.width: 1
                            border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                            
                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                            Behavior on color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: 11
                            color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
                
                // Words-based options (shown when mode is "all" or "words")
                Repeater {
                    model: (root.selectedModeFilter === "all" || root.selectedModeFilter === "words") ? [10, 25, 50, 100] : []
                    
                    Button {
                        text: modelData + "w"
                        checkable: true
                        checked: root.selectedDurationFilter === modelData
                        hoverEnabled: true
                        
                        onClicked: {
                            root.selectedDurationFilter = modelData
                            root.filtersChanged()
                        }
                        
                        background: Rectangle {
                            implicitWidth: 45
                            implicitHeight: 30
                            color: parent.checked ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                            radius: 6
                            border.width: 1
                            border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                            
                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                            Behavior on color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: 11
                            color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        } // Column - Duration/WordCount Filter
        
        Item { 
            Layout.fillWidth: true 
            visible: root.width >= 600
        }
    } // RowLayout
}
