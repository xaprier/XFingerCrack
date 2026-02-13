import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Popup {
    id: settingsPopup
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    
    property string previousScreen: "typing"
    
    anchors.centerIn: Overlay.overlay
    width: Math.min(600, parent.width - 40)
    height: Math.min(500, parent.height - 40)
    
    background: Rectangle {
        color: themeManager.currentTheme.background
        border.color: themeManager.currentTheme.border
        border.width: 2
        radius: 8
    }
    
    onOpened: {
        settingsList.forceActiveFocus()
        if (settingsList.count > 0) {
            settingsList.currentIndex = 0
        }
    }
    
    onClosed: {
        searchField.text = ""
        settingsList.currentIndex = -1
        optionsList.visible = false
        
        // Restore previous screen
        if (previousScreen === "history") {
            showHistory = true
        } else if (previousScreen === "results") {
            showResults = true
        } else {
            typingArea.forceActiveFocus()
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Text {
                text: qsTr("Settings")
                font.pixelSize: 24
                font.bold: true
                color: themeManager.currentTheme.text
                Layout.fillWidth: true
            }
            
            // Close button
            Rectangle {
                width: 32
                height: 32
                radius: 16
                color: closeMouse.containsMouse ? themeManager.currentTheme.error : themeManager.currentTheme.surface
                border.color: themeManager.currentTheme.border
                border.width: 1
                
                Text {
                    anchors.centerIn: parent
                    text: "×"
                    font.pixelSize: 20
                    color: themeManager.currentTheme.text
                }
                
                MouseArea {
                    id: closeMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: settingsPopup.close()
                }
            }
        }
        
        // Search field (display only)
        Rectangle {
            Layout.fillWidth: true
            height: 40
            radius: 6
            color: themeManager.currentTheme.surface
            border.color: themeManager.currentTheme.border
            border.width: 1
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8
                
                Text {
                    text: "🔍"
                    font.pixelSize: 16
                }
                
                Text {
                    id: searchDisplayText
                    Layout.fillWidth: true
                    text: searchField.text.length > 0 ? searchField.text : qsTr("Search settings...")
                    color: searchField.text.length > 0 ? themeManager.currentTheme.text : themeManager.currentTheme.textSecondary
                    font.pixelSize: 14
                    opacity: searchField.text.length > 0 ? 1.0 : 0.6
                }
            }
        }
        
        // Hidden TextField for search functionality
        TextField {
            id: searchField
            visible: false
            text: ""
            
            onTextChanged: {
                filterSettings()
                // Reset to first item when search changes
                if (settingsList.count > 0) {
                    settingsList.currentIndex = 0
                }
            }
        }
        
        // Content area with settings and options
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            // Settings list (left side)
            Rectangle {
                id: settingsContainer
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: optionsList.visible ? parent.width * 0.45 : parent.width
                color: themeManager.currentTheme.surface
                radius: 6
                border.color: themeManager.currentTheme.border
                border.width: 1
                
                Behavior on width {
                    NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
                }
                
                ListView {
                    id: settingsList
                    anchors.fill: parent
                    anchors.margins: 5
                    clip: true
                    spacing: 2
                    
                    model: filteredSettingsModel
                    
                    delegate: SettingsItem {
                        width: settingsList.width
                        icon: model.icon
                        name: model.name
                        description: model.description
                        key: model.key
                        isCurrentItem: ListView.isCurrentItem
                        index: model.index
                        
                        onClicked: {
                            settingsList.currentIndex = index
                            showOptions(key)
                        }
                    }
                    
                    Keys.onPressed: (event) => {
                        // Handle backspace to remove last character from search
                        if (event.key === Qt.Key_Backspace) {
                            if (searchField.text.length > 0) {
                                searchField.text = searchField.text.slice(0, -1)
                            }
                            event.accepted = true
                            return
                        }
                        
                        // Handle delete to clear all search
                        if (event.key === Qt.Key_Delete) {
                            searchField.text = ""
                            event.accepted = true
                            return
                        }
                        
                        // Handle text input for search (alphanumeric and space)
                        if (event.text.length > 0 && !event.modifiers) {
                            if (event.text.match(/[a-zA-Z0-9 ]/)) {
                                searchField.text += event.text
                                event.accepted = true
                            }
                        }
                    }
                    
                    Keys.onEscapePressed: {
                        if (searchField.text.length > 0) {
                            // Clear search first
                            searchField.text = ""
                        } else if (optionsList.visible) {
                            optionsList.visible = false
                            settingsList.currentIndex = -1
                        } else {
                            settingsPopup.close()
                        }
                    }
                    
                    Keys.onReturnPressed: {
                        if (currentIndex >= 0) {
                            let item = filteredSettingsModel.get(currentIndex)
                            showOptions(item.key)
                        }
                    }
                }
            }
            
            // Options list (right side)
            Rectangle {
                id: optionsList
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: parent.width * 0.52
                color: themeManager.currentTheme.background
                radius: 6
                border.color: themeManager.currentTheme.accent
                border.width: 2
                visible: false
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 8
                    
                    // Options header
                    RowLayout {
                        Layout.fillWidth: true
                        
                        Rectangle {
                            width: 28
                            height: 28
                            radius: 4
                            color: backMouse.containsMouse ? Qt.lighter(themeManager.currentTheme.surface, 1.2) : themeManager.currentTheme.surface
                            border.color: themeManager.currentTheme.border
                            border.width: 1
                            
                            Text {
                                anchors.centerIn: parent
                                text: "←"
                                font.pixelSize: 16
                                color: themeManager.currentTheme.text
                            }
                            
                            MouseArea {
                                id: backMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    optionsList.visible = false
                                    settingsList.currentIndex = previousSettingsIndex
                                    settingsList.forceActiveFocus()
                                }
                            }
                        }
                        
                        Text {
                            text: currentSettingName
                            font.pixelSize: 16
                            font.bold: true
                            color: themeManager.currentTheme.text
                            Layout.fillWidth: true
                        }
                    }
                    
                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: themeManager.currentTheme.border
                    }
                    
                    // Options ListView
                    ListView {
                        id: optionsListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 3
                        focus: true
                        
                        model: currentOptionsModel
                        
                        Keys.onPressed: (event) => {
                            if (event.key === Qt.Key_Backspace) {
                                optionsList.visible = false
                                settingsList.currentIndex = previousSettingsIndex
                                settingsList.forceActiveFocus()
                                event.accepted = true
                            }
                        }
                        
                        Keys.onEscapePressed: {
                            optionsList.visible = false
                            settingsList.currentIndex = previousSettingsIndex
                            settingsList.forceActiveFocus()
                        }
                        
                        Keys.onReturnPressed: {
                            if (currentIndex >= 0) {
                                let item = currentOptionsModel.get(currentIndex)
                                applyOption(currentSettingKey, item.value)
                                updateOptionsModel()
                            }
                        }
                        
                        highlightFollowsCurrentItem: true
                        highlight: Rectangle {
                            color: Qt.rgba(themeManager.currentTheme.accent.r, themeManager.currentTheme.accent.g, themeManager.currentTheme.accent.b, 0.3)
                            radius: 4
                            border.color: themeManager.currentTheme.accent
                            border.width: 2
                        }
                        
                        delegate: SettingsOptionItem {
                            width: optionsListView.width
                            label: model.label
                            value: model.value
                            isSelected: model.isSelected
                            isCurrentItem: ListView.isCurrentItem
                            
                            onClicked: {
                                applyOption(currentSettingKey, value)
                                updateOptionsModel()
                            }
                        }
                    }
                }
            }
        }
        
        // Footer with keyboard shortcuts
        Rectangle {
            Layout.fillWidth: true
            height: 30
            radius: 4
            color: themeManager.currentTheme.surface
            
            Text {
                anchors.centerIn: parent
                text: qsTr("Type to search • ESC: back/close • ↓↑: navigate • Enter: select • Del: clear search")
                font.pixelSize: 10
                color: themeManager.currentTheme.textSecondary
                opacity: 0.7
            }
        }
    }
    
    // Listen for language changes
    Connections {
        target: translationManager
        function onCurrentLanguageChanged() {
            populateSettingsModel()
            // Update current options if visible
            if (optionsList.visible) {
                showOptions(currentSettingKey)
            }
        }
    }
    
    // Settings data model
    ListModel {
        id: allSettingsModel
    }
    
    // Filtered model for search
    ListModel {
        id: filteredSettingsModel
    }
    
    // Current options model
    ListModel {
        id: currentOptionsModel
    }
    
    property string currentSettingKey: ""
    property string currentSettingName: ""
    property int previousSettingsIndex: -1
    
    Component.onCompleted: {
        // Initialize settings model with translations
        populateSettingsModel()
        // Initialize filtered model
        filterSettings()
    }
    
    function populateSettingsModel() {
        // Save currently selected item key to restore after repopulating
        let selectedKey = ""
        if (settingsList.currentIndex >= 0 && settingsList.currentIndex < filteredSettingsModel.count) {
            selectedKey = filteredSettingsModel.get(settingsList.currentIndex).key
        }
        
        allSettingsModel.clear()
        allSettingsModel.append({
            key: "mode",
            name: qsTr("Test Mode"),
            description: qsTr("Time or words based testing"),
            icon: "⏱️"
        })
        allSettingsModel.append({
            key: "duration",
            name: qsTr("Test Duration"),
            description: qsTr("Time limit for time-based tests"),
            icon: "⏰"
        })
        allSettingsModel.append({
            key: "wordCount",
            name: qsTr("Word Count"),
            description: qsTr("Number of words for word-based tests"),
            icon: "📝"
        })
        allSettingsModel.append({
            key: "theme",
            name: qsTr("Theme"),
            description: qsTr("Visual appearance"),
            icon: "🎨"
        })
        allSettingsModel.append({
            key: "language",
            name: qsTr("Language"),
            description: qsTr("Word list language"),
            icon: "🌍"
        })
        allSettingsModel.append({
            key: "uiLanguage",
            name: qsTr("UI Language"),
            description: qsTr("Interface language"),
            icon: "🌐"
        })
        allSettingsModel.append({
            key: "soundEnabled",
            name: qsTr("Sound"),
            description: qsTr("Enable or disable all sounds"),
            icon: "🔊"
        })
        allSettingsModel.append({
            key: "clickSoundVariant",
            name: qsTr("Click Sound Type"),
            description: qsTr("Keyboard sound profile"),
            icon: "⌨️"
        })
        allSettingsModel.append({
            key: "soundVolume",
            name: qsTr("Sound Volume"),
            description: qsTr("Adjust volume level (0-100%)"),
            icon: "🔉"
        })
        allSettingsModel.append({
            key: "soundOnClick",
            name: qsTr("Click Sound"),
            description: qsTr("Play sound on key press"),
            icon: "🎵"
        })
        allSettingsModel.append({
            key: "soundOnError",
            name: qsTr("Error Sound"),
            description: qsTr("Play sound on typing error"),
            icon: "🔔"
        })
        allSettingsModel.append({
            key: "about",
            name: qsTr("About"),
            description: qsTr("Application and developer information"),
            icon: "ℹ️"
        })
        
        // Refresh filtered list after populating
        filterSettings()
        
        // Restore selection if we had one
        if (selectedKey !== "") {
            for (let i = 0; i < filteredSettingsModel.count; i++) {
                if (filteredSettingsModel.get(i).key === selectedKey) {
                    settingsList.currentIndex = i
                    break
                }
            }
        }
    }
    
    function filterSettings() {
        filteredSettingsModel.clear()
        let searchText = searchField.text.toLowerCase()
        
        for (let i = 0; i < allSettingsModel.count; i++) {
            let item = allSettingsModel.get(i)
            if (searchText === "" || 
                item.name.toLowerCase().includes(searchText) || 
                item.description.toLowerCase().includes(searchText)) {
                filteredSettingsModel.append(item)
            }
        }
    }
    
    function showOptions(settingKey) {
        // Save current settings list index before showing options
        previousSettingsIndex = settingsList.currentIndex
        
        currentSettingKey = settingKey
        currentOptionsModel.clear()
        
        switch(settingKey) {
            case "mode":
                currentSettingName = qsTr("Test Mode")
                currentOptionsModel.append({
                    label: qsTr("Time"),
                    value: "0",
                    isSelected: configManager.mode === 0
                })
                currentOptionsModel.append({
                    label: qsTr("Words"),
                    value: "1",
                    isSelected: configManager.mode === 1
                })
                break
                
            case "duration":
                currentSettingName = qsTr("Test Duration")
                let durations = [15, 30, 60, 120]
                for (let d of durations) {
                    currentOptionsModel.append({
                        label: d + qsTr(" seconds"),
                        value: d.toString(),
                        isSelected: configManager.duration === d
                    })
                }
                break
                
            case "wordCount":
                currentSettingName = qsTr("Word Count")
                let counts = [10, 25, 50, 100]
                for (let c of counts) {
                    currentOptionsModel.append({
                        label: c + qsTr(" words"),
                        value: c.toString(),
                        isSelected: configManager.wordCount === c
                    })
                }
                break
                
            case "theme":
                currentSettingName = qsTr("Theme")
                let themes = themeManager.availableThemes
                for (let i = 0; i < themes.length; i++) {
                    let themeObj = themes[i]
                    currentOptionsModel.append({
                        label: themeObj.name,
                        value: themeObj.id,
                        isSelected: configManager.theme === themeObj.id
                    })
                }
                break
                
            case "language":
                currentSettingName = qsTr("Language")
                let languages = wordGenerator.AvailableLanguages()
                for (let lang of languages) {
                    currentOptionsModel.append({
                        label: lang.charAt(0).toUpperCase() + lang.slice(1),
                        value: lang,
                        isSelected: configManager.language === lang
                    })
                }
                break
                
            case "uiLanguage":
                currentSettingName = qsTr("UI Language")
                let uiLanguages = translationManager.AvailableLanguages()
                for (let lang of uiLanguages) {
                    currentOptionsModel.append({
                        label: translationManager.LanguageDisplayName(lang),
                        value: lang,
                        isSelected: configManager.uiLanguage === lang
                    })
                }
                break
                
            case "soundEnabled":
                currentSettingName = qsTr("Sound")
                currentOptionsModel.append({
                    label: qsTr("Enabled"),
                    value: "true",
                    isSelected: configManager.soundEnabled === true
                })
                currentOptionsModel.append({
                    label: qsTr("Disabled"),
                    value: "false",
                    isSelected: configManager.soundEnabled === false
                })
                break
                
            case "clickSoundVariant":
                currentSettingName = qsTr("Click Sound Type")
                let variants = ["click4", "click5", "click6", "click15"]
                let variantNames = {
                    "click4": qsTr("Keyboard Type 1"),
                    "click5": qsTr("Keyboard Type 2"),
                    "click6": qsTr("Keyboard Type 3"),
                    "click15": qsTr("Keyboard Type 4")
                }
                for (let v of variants) {
                    currentOptionsModel.append({
                        label: variantNames[v],
                        value: v,
                        isSelected: configManager.clickSoundVariant === v
                    })
                }
                break
                
            case "soundVolume":
                currentSettingName = qsTr("Sound Volume")
                let volumes = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
                for (let v of volumes) {
                    let volumeValue = v / 100.0
                    currentOptionsModel.append({
                        label: v + "%",
                        value: volumeValue.toFixed(2),
                        isSelected: Math.abs(configManager.soundVolume - volumeValue) < 0.01
                    })
                }
                break
                
            case "soundOnClick":
                currentSettingName = qsTr("Click Sound")
                currentOptionsModel.append({
                    label: qsTr("Enabled"),
                    value: "true",
                    isSelected: configManager.soundOnClick === true
                })
                currentOptionsModel.append({
                    label: qsTr("Disabled"),
                    value: "false",
                    isSelected: configManager.soundOnClick === false
                })
                break
                
            case "soundOnError":
                currentSettingName = qsTr("Error Sound")
                currentOptionsModel.append({
                    label: qsTr("Enabled"),
                    value: "true",
                    isSelected: configManager.soundOnError === true
                })
                currentOptionsModel.append({
                    label: qsTr("Disabled"),
                    value: "false",
                    isSelected: configManager.soundOnError === false
                })
                break
                
            case "about":
                // Open About popup instead of showing options
                settingsPopup.visible = false
                aboutPopup.open()
                return
        }
        
        optionsList.visible = true
        
        // Find selected item index and set it as current
        let selectedIndex = 0
        for (let i = 0; i < currentOptionsModel.count; i++) {
            if (currentOptionsModel.get(i).isSelected) {
                selectedIndex = i
                break
            }
        }
        
        optionsListView.currentIndex = selectedIndex
        optionsListView.forceActiveFocus()
    }
    
    function applyOption(settingKey, value) {
        switch(settingKey) {
            case "mode":
                configManager.mode = parseInt(value)
                break
            case "duration":
                configManager.duration = parseInt(value)
                break
            case "wordCount":
                configManager.wordCount = parseInt(value)
                break
            case "theme":
                themeManager.SetTheme(value)
                configManager.theme = value
                break
            case "language":
                configManager.language = value
                break
            case "uiLanguage":
                configManager.uiLanguage = value
                break
            case "soundEnabled":
                configManager.soundEnabled = (value === "true")
                break
            case "clickSoundVariant":
                configManager.clickSoundVariant = value
                break
            case "soundVolume":
                configManager.soundVolume = parseFloat(value)
                break
            case "soundOnClick":
                configManager.soundOnClick = (value === "true")
                break
            case "soundOnError":
                configManager.soundOnError = (value === "true")
                break
        }
    }
    
    function updateOptionsModel() {
        showOptions(currentSettingKey)
    }
}
