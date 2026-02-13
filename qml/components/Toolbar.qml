import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: toolbar
    height: Math.max(60, toolbarFlow.height + 30)
    color: themeManager.currentTheme.surface
    
    property int minimumContentWidth: 1090
    
    Flow {
        id: toolbarFlow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 15
        spacing: 20

            // spacer
        Item {
            width: 0
            height: 30
            Layout.fillWidth: true
        }
            // Logo
            Item {
                width: logoText.width
                height: 30
                
                Text {
                    id: logoText
                    text: appInfo.applicationName
                    font.pixelSize: 20
                    font.bold: true
                    color: themeManager.currentTheme.accent
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            
            Rectangle {
                width: 1
                height: 25
                color: themeManager.currentTheme.border
            }
            
            // Mode Section
            Item {
                width: modeLayout.width
                height: 30
                
                RowLayout {
                    id: modeLayout
                    spacing: 5
                    anchors.verticalCenter: parent.verticalCenter
                    
                    Text {
                        text: qsTr("Mode")
                        color: themeManager.currentTheme.textSecondary
                        font.pixelSize: 12
                    }
                    
                    Button {
                        text: qsTr("Time")
                        flat: true
                        checkable: false
                        checked: configManager.mode === 0
                        focusPolicy: Qt.NoFocus
                        hoverEnabled: true
                        implicitWidth: 55
                        implicitHeight: 28
                        onClicked: {
                            showHistory = false
                            showResults = false
                            configManager.mode = 0
                            typingTest.mode = 0
                            typingTest.ResetTest()
                            typingArea.forceActiveFocus()
                        }
                        
                        background: Rectangle {
                            color: parent.checked ? themeManager.currentTheme.accent : "transparent"
                            radius: 4
                            border.color: parent.checked ? themeManager.currentTheme.border : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                            border.width: parent.checked ? 0 : 1
                            
                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                    }
                    
                    Button {
                        text: qsTr("Words")
                        flat: true
                        checkable: false
                        checked: configManager.mode === 1
                        focusPolicy: Qt.NoFocus
                        hoverEnabled: true
                        implicitWidth: 55
                        implicitHeight: 28
                        onClicked: {
                            showHistory = false
                            showResults = false
                            configManager.mode = 1
                            typingTest.mode = 1
                            typingTest.ResetTest()
                            typingArea.forceActiveFocus()
                        }
                        
                        background: Rectangle {
                            color: parent.checked ? themeManager.currentTheme.accent : "transparent"
                            radius: 4
                            border.color: parent.checked ? themeManager.currentTheme.border : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                            border.width: parent.checked ? 0 : 1
                            
                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                    }
                }
            }
            
            // Duration Section (Time Mode)
            Item {
                visible: configManager.mode === 0
                width: visible ? durationLayout.width : 0
                height: 30
                
                RowLayout {
                    id: durationLayout
                    spacing: 4
                    anchors.verticalCenter: parent.verticalCenter
                    
                    Repeater {
                        model: [15, 30, 60, 120]
                        
                        Button {
                            text: modelData + "s"
                            flat: true
                            checkable: false
                            checked: configManager.duration === modelData
                            focusPolicy: Qt.NoFocus
                            implicitWidth: 42
                            implicitHeight: 28
                            onClicked: {
                                showHistory = false
                                showResults = false
                                configManager.duration = modelData
                                typingTest.duration = modelData
                                typingTest.ResetTest()
                                typingArea.forceActiveFocus()
                            }
                            
                            background: Rectangle {
                                color: parent.checked ? themeManager.currentTheme.accent : "transparent"
                                radius: 4
                                border.color: parent.checked ? themeManager.currentTheme.border : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                                border.width: parent.checked ? 0 : 1
                                
                                Behavior on border.color {
                                    ColorAnimation { duration: 150 }
                                }
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }
            
            // Word Count Section (Words Mode)
            Item {
                visible: configManager.mode === 1
                width: visible ? wordCountLayout.width : 0
                height: 30
                
                RowLayout {
                    id: wordCountLayout
                    spacing: 4
                    anchors.verticalCenter: parent.verticalCenter
                    
                    Repeater {
                        model: [10, 25, 50, 100]
                        
                        Button {
                            text: modelData.toString()
                            flat: true
                            checkable: false
                            checked: configManager.wordCount === modelData
                            focusPolicy: Qt.NoFocus
                            hoverEnabled: true
                            implicitWidth: 42
                            implicitHeight: 28
                            onClicked: {
                                showHistory = false
                                showResults = false
                                configManager.wordCount = modelData
                                typingTest.wordCount = modelData
                                typingTest.ResetTest()
                                typingArea.forceActiveFocus()
                            }
                            
                            background: Rectangle {
                                color: parent.checked ? themeManager.currentTheme.accent : "transparent"
                                radius: 4
                                border.color: parent.checked ? themeManager.currentTheme.border : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                                border.width: parent.checked ? 0 : 1
                                
                                Behavior on border.color {
                                    ColorAnimation { duration: 150 }
                                }
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: parent.checked ? themeManager.currentTheme.background : themeManager.currentTheme.text
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }
            
            Rectangle {
                width: 1
                height: 25
                color: themeManager.currentTheme.border
            }
            
            // Theme Section
            Item {
                width: themeLayout.width
                height: 30
                
                RowLayout {
                    id: themeLayout
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter
                    
                    Text {
                        text: qsTr("Theme")
                        color: themeManager.currentTheme.textSecondary
                        font.pixelSize: 12
                    }
                    
                    ComboBox {
                        id: themeCombo
                        implicitWidth: 105
                        implicitHeight: 28
                        model: themeManager.availableThemes
                        textRole: "name"
                        focusPolicy: Qt.NoFocus
                        
                        Component.onCompleted: {
                            updateThemeIndex()
                        }
                        
                        Connections {
                            target: themeManager
                            function onCurrentThemeIdChanged() {
                                themeCombo.updateThemeIndex()
                            }
                        }
                        
                        function updateThemeIndex() {
                            for (var i = 0; i < themeManager.availableThemes.length; i++) {
                                if (themeManager.availableThemes[i].id === themeManager.currentThemeId) {
                                    themeCombo.currentIndex = i
                                    return
                                }
                            }
                            themeCombo.currentIndex = 0
                        }
                        
                        onActivated: {
                            var theme = themeManager.availableThemes[currentIndex]
                            themeManager.SetTheme(theme.id)
                            configManager.theme = theme.id
                            typingArea.forceActiveFocus()
                        }

                        background: Rectangle {
                            color: themeManager.currentTheme.surface
                            radius: 4
                            border.width: 1
                            border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                            
                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: Text {
                            text: themeCombo.displayText
                            color: themeManager.currentTheme.text
                            font.pixelSize: 11
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: 8
                            rightPadding: 8
                            elide: Text.ElideRight
                        }
                        
                        delegate: ItemDelegate {
                            width: themeCombo.width
                            height: 28
                            
                            background: Rectangle {
                                color: highlighted ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                            }
                            
                            contentItem: Text {
                                text: modelData.name
                                color: highlighted ? themeManager.currentTheme.background : themeManager.currentTheme.text
                                font.pixelSize: 11
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 8
                            }
                        }
                        
                        popup: Popup {
                            y: themeCombo.height
                            width: themeCombo.width
                            implicitHeight: contentItem.implicitHeight
                            padding: 1
                            
                            onClosed: {
                                typingArea.forceActiveFocus()
                            }
                            
                            background: Rectangle {
                                color: themeManager.currentTheme.surface
                                border.color: themeManager.currentTheme.border
                                border.width: 1
                                radius: 4
                            }
                            
                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                model: themeCombo.popup.visible ? themeCombo.delegateModel : null
                                currentIndex: themeCombo.highlightedIndex
                            }
                        }
                    }
                }
            }
            
            // Language Section
            Item {
                width: languageLayout.width
                height: 30
                
                RowLayout {
                    id: languageLayout
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter
                    
                    Text {
                        text: qsTr("Language")
                        color: themeManager.currentTheme.textSecondary
                        font.pixelSize: 12
                    }
                    
                    ComboBox {
                        id: languageCombo
                        implicitWidth: 105
                        implicitHeight: 28
                        model: wordGenerator.AvailableLanguages()
                        focusPolicy: Qt.NoFocus
                        
                        Component.onCompleted: {
                            updateLanguageIndex()
                        }
                        
                        Connections {
                            target: configManager
                            function onLanguageChanged() {
                                languageCombo.updateLanguageIndex()
                            }
                        }
                        
                        function updateLanguageIndex() {
                            var langs = wordGenerator.AvailableLanguages()
                            for (var i = 0; i < langs.length; i++) {
                                if (langs[i] === configManager.language) {
                                    languageCombo.currentIndex = i
                                    return
                                }
                            }
                            languageCombo.currentIndex = 0
                        }
                        
                        onActivated: {
                            showHistory = false
                            showResults = false
                            var language = model[currentIndex]
                            configManager.language = language
                            typingArea.forceActiveFocus()
                        }
                        
                        background: Rectangle {
                            color: themeManager.currentTheme.surface
                            radius: 4
                            border.width: 1
                            border.color: parent.checked ? themeManager.currentTheme.accent : (parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border)
                            
                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        contentItem: Text {
                            text: languageCombo.displayText
                            color: themeManager.currentTheme.text
                            font.pixelSize: 11
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: 8
                            rightPadding: 8
                            elide: Text.ElideRight
                        }
                        
                        delegate: ItemDelegate {
                            width: languageCombo.width
                            height: 28
                            
                            background: Rectangle {
                                color: highlighted ? themeManager.currentTheme.accent : themeManager.currentTheme.surface
                            }
                            
                            contentItem: Text {
                                text: modelData
                                color: highlighted ? themeManager.currentTheme.background : themeManager.currentTheme.text
                                font.pixelSize: 11
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 8
                            }
                        }
                        
                        popup: Popup {
                            y: languageCombo.height
                            width: languageCombo.width
                            implicitHeight: contentItem.implicitHeight
                            padding: 1
                            
                            onClosed: {
                                typingArea.forceActiveFocus()
                            }
                            
                            background: Rectangle {
                                color: themeManager.currentTheme.surface
                                border.color: themeManager.currentTheme.border
                                border.width: 1
                                radius: 4
                            }
                            
                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                model: languageCombo.popup.visible ? languageCombo.delegateModel : null
                                currentIndex: languageCombo.highlightedIndex
                            }
                        }
                    }
                }
            
            }
            
            Rectangle {
                width: 1
                height: 25
                color: themeManager.currentTheme.border
            }
            
            // Settings Button
            Button {
                id: settingsButton
                width: 35
                height: 30
                hoverEnabled: true
                
                ThemedToolTip {
                    visible: parent.hovered
                    text: qsTr("Settings") + " (Esc)"
                }
                
                contentItem: Item {
                    Text {
                        anchors.centerIn: parent
                        text: "⚙️"
                        font.pixelSize: 18
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                background: Rectangle {
                    color: parent.pressed ? Qt.darker(themeManager.currentTheme.surface, 1.2) : 
                           parent.hovered ? Qt.lighter(themeManager.currentTheme.surface, 1.1) : 
                           themeManager.currentTheme.surface
                    radius: 6
                    border.width: 1
                    border.color: parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border
                    
                    Behavior on border.color {
                        ColorAnimation { duration: 150 }
                    }
                }
                
                onClicked: {
                    settingsPopup.previousScreen = showHistory ? "history" : (showResults ? "results" : "typing")
                    settingsPopup.open()
                }
            }
            
            Rectangle {
                width: 1
                height: 25
                color: themeManager.currentTheme.border
            }
            
            // Profile/History Button
            Button {
                id: profileButton
                width: 35
                height: 30
                hoverEnabled: true
                
                ThemedToolTip {
                    visible: parent.hovered
                    text: qsTr("Test History")
                }
                
                contentItem: Item {
                    Text {
                        anchors.centerIn: parent
                        text: "👤"
                        font.pixelSize: 18
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                
                background: Rectangle {
                    color: parent.pressed ? Qt.darker(themeManager.currentTheme.surface, 1.2) : 
                           parent.hovered ? Qt.lighter(themeManager.currentTheme.surface, 1.1) : 
                           themeManager.currentTheme.surface
                    radius: 6
                    border.width: 1
                    border.color: parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border
                    
                    Behavior on border.color {
                        ColorAnimation { duration: 150 }
                    }
                }
                
                onClicked: {
                    showHistory = true
                }
            }
    }
}