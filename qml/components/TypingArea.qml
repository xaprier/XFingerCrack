import QtQuick
import QtQuick.Controls
import QtMultimedia

Item {
    id: root
    
    property var skippedIndices: new Set()
    property var clickSounds: []
    property int soundIndex: 0
    
    // ===== CENTRALIZED COMPUTED PROPERTIES - Calculated ONCE per keystroke =====
    // These avoid re-splitting and re-parsing in every Text item binding
    property var _userWords: []
    property var _targetWords: []
    property var _wordStartOffsets: []  // Cached start index for each word
    property int _spacesTyped: 0
    
    // Global current word index - now just uses cached _spacesTyped
    property int currentWordIndex: _spacesTyped
    
    // Current character position within the current word
    property int currentCharacterIndexInWord: {
        var userWord = currentWordIndex < _userWords.length ? _userWords[currentWordIndex] : ""
        return userWord.length
    }
    
    // Recalculate cached values when userInput changes
    function recalcUserInputCache() {
        var ui = typingTest.userInput
        _userWords = ui.split(" ")
        
        var c = 0
        for (var i = 0; i < ui.length; ++i) {
            if (ui.charAt(i) === ' ') c++
        }
        _spacesTyped = c
    }
    
    // Recalculate target text cached values
    function recalcTargetCache() {
        _targetWords = typingTest.targetText.split(" ")
        
        // Calculate start offset for each word
        _wordStartOffsets = []
        var pos = 0
        for (var i = 0; i < _targetWords.length; ++i) {
            _wordStartOffsets.push(pos)
            pos += _targetWords[i].length + 1  // +1 for space
        }
    }
    
    // Sound effects
    SoundEffect {
        id: errorSound
        source: "qrc:/sounds/error2/error2_1.wav"
        volume: configManager.soundVolume
    }
    
    // Initialize click sounds based on variant
    Component.onCompleted: {
        recalcTargetCache()
        recalcUserInputCache()
        loadClickSounds()
        inputField.forceActiveFocus()
        // Initialize scroll position
        wordFlow.updateScroll()
    }
    
    Connections {
        target: configManager
        function onClickSoundVariantChanged() {
            loadClickSounds()
        }
        function onSoundVolumeChanged() {
            loadClickSounds()
        }
    }
    
    // Update cached values when typingTest properties change
    Connections {
        target: typingTest
        function onTargetTextChanged() {
            recalcTargetCache()
        }
    }
    
    function loadClickSounds() {
        let variant = configManager.clickSoundVariant
        let basePath = "qrc:/sounds/" + variant + "/"
        
        // Clear existing sounds
        for (let i = 0; i < clickSounds.length; i++) {
            if (clickSounds[i]) {
                clickSounds[i].destroy()
            }
        }
        clickSounds = []
        
        // Load sound files based on variant
        let soundFiles = []
        if (variant === "click4" || variant === "click5") {
            soundFiles = ["1", "2", "3", "4", "5", "6", "11", "22", "33", "44", "55", "66"]
        } else if (variant === "click6") {
            soundFiles = ["1", "2", "3", "11", "22", "33"]
        } else if (variant === "click15") {
            soundFiles = ["1", "2", "3", "4", "5"]
        }
        
        for (let i = 0; i < soundFiles.length; i++) {
            try {
                let soundPath = basePath + variant + '_' + soundFiles[i] + '.wav'
                let sound = Qt.createQmlObject(
                    'import QtMultimedia; SoundEffect { source: "' + soundPath + '"; volume: ' + configManager.soundVolume + ' }',
                    root,
                    "clickSound" + i
                )
                clickSounds.push(sound)
            } catch (e) {
                console.error("Failed to load sound:", e)
            }
        }
        
        soundIndex = 0
    }
    
    function playClickSound() {
        if (configManager.soundEnabled && configManager.soundOnClick && clickSounds.length > 0) {
            clickSounds[soundIndex].play()
            soundIndex = (soundIndex + 1) % clickSounds.length
        } 
    }
    
    function playErrorSound() {
        if (configManager.soundEnabled && configManager.soundOnError) {
            errorSound.play()
        } 
    }
    
    // Expose inputField for focus management
    function forceActiveFocus() {
        inputField.forceActiveFocus()
    }
    
    // Track if we're updating from code to avoid feedback loop
    property bool _updatingFromCode: false
    
    // Reset scroll position and state when test resets
    Connections {
        target: typingTest
        function onUserInputChanged() {
            // Update cached values
            recalcUserInputCache()
            
            // When test is reset (userInput becomes empty), reset scroll state
            if (typingTest.userInput === "") {
                wordFlow.y = 0
                wordFlow.lastScrolledLine = 0
                root.skippedIndices = new Set()
                
                // Avoid feedback loop: only update TextField if not already synchronizing
                if (!_updatingFromCode && inputField.text !== "") {
                    _updatingFromCode = true
                    inputField.text = ""
                    _updatingFromCode = false
                }
            }
        }
    }
    
    // Add focus border
    Rectangle {
        anchors.fill: parent
        anchors.margins: -10  // Extend slightly outside
        color: "transparent"
        border.color: inputField.activeFocus ? themeManager.currentTheme.accent : "transparent"
        border.width: 2
        radius: 12
        z: 1000
    }
    
    Column {
        anchors.centerIn: parent
        anchors.margins: 15  // Padding inside border
        spacing: 40
        width: parent.width - 30  // Account for margins
        
        // Stats display
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 30
            
            // Timer or word count
            Column {
                spacing: 5
                
                Text {
                    text: typingTest.mode === 0 ? qsTr("Time") : qsTr("Words")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 14
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                
                Text {
                    text: typingTest.mode === 0 ? 
                        typingTest.remainingTime + qsTr("s") : 
                        typingTest.remainingWords + qsTr(" left")
                    color: themeManager.currentTheme.accent
                    font.pixelSize: 28
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
            
            Rectangle {
                width: 2
                height: 50
                color: themeManager.currentTheme.border
                anchors.verticalCenter: parent.verticalCenter
            }
            
            // WPM
            Column {
                spacing: 5
                
                Text {
                    text: qsTr("WPM")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 14
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                
                Text {
                    text: typingTest.liveWpm
                    color: themeManager.currentTheme.text
                    font.pixelSize: 28
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
            
            Rectangle {
                width: 2
                height: 50
                color: themeManager.currentTheme.border
                anchors.verticalCenter: parent.verticalCenter
            }
            
            // Accuracy
            Column {
                spacing: 5
                
                Text {
                    text: qsTr("Accuracy")
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 14
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                
                Text {
                    text: typingTest.liveAccuracy.toFixed(1) + "%"
                    color: themeManager.currentTheme.text
                    font.pixelSize: 28
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
        
        // Text display area
        Rectangle {
            width: parent.width
            height: 24 * 1.5 * 3 + 60 // font size * line height * 3 lines + margins
            color: themeManager.currentTheme.surface
            radius: 10
            border.color: themeManager.currentTheme.border
            border.width: 2
            
            Item {
                id: textContainer
                anchors.fill: parent
                anchors.margins: 30
                clip: true
                
                Flow {
                    id: wordFlow
                    width: parent.width
                    spacing: 8
                    y: 0
                    
                    property int lastScrolledLine: 0
                    property real exactLineHeight: 36.0 // Exact line height (24px font * 1.5 spacing)
                    
                    Behavior on y {
                        NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
                    }
                    
                    function updateScroll() {
                        // Calculate vertical offset to keep current word on top line
                        // Use pre-computed currentWordIndex instead of re-counting spaces
                        var currentWordIndex = root.currentWordIndex
                        
                        // Find the item for current word
                        if (currentWordIndex < wordRepeater.count) {
                            var item = wordRepeater.itemAt(currentWordIndex)
                            if (item) {
                                // Calculate which line it's on
                                var currentLine = Math.floor(item.y / exactLineHeight)
                                
                                // Only scroll if we've moved to a new line
                                if (currentLine !== lastScrolledLine) {
                                    lastScrolledLine = currentLine
                                    // Always scroll by exact multiples of line height
                                    wordFlow.y = -currentLine * exactLineHeight
                                }
                            }
                        }
                    }
                    
                    Repeater {
                        id: wordRepeater
                        model: typingTest.targetText.split(" ")
                        
                        Item {
                            width: wordRow.width
                            height: wordRow.height
                            
                            property string wordText: modelData
                            property int wordIndex: index
                            // Use pre-computed start offsets instead of calculating
                            property int wordStartIndex: root._wordStartOffsets[index] || 0
                            property int wordEndIndex: wordStartIndex + wordText.length
                            property int currentPos: typingTest.userInput.length
                            // Current word is determined by global currentWordIndex
                            property bool isCurrentWord: wordIndex === root.currentWordIndex
                            property int wordCharWidth: wordText.length * 14 // Approximate character width
                            
                            // Word background for current word
                            Rectangle {
                                visible: isCurrentWord
                                x: -4
                                y: -2
                                width: {
                                    // Calculate width based only on word characters + extra chars, not space
                                    var baseWidth = wordText.length * 14.5 // Approximate char width
                                    // Use cached _userWords instead of re-splitting
                                    var userWord = wordIndex < root._userWords.length ? root._userWords[wordIndex] : ""
                                    var extraChars = Math.max(0, userWord.length - wordText.length)
                                    return baseWidth + (extraChars * 14.5) + 8 // +8 for padding
                                }
                                height: wordRow.height + 4
                                color: themeManager.currentTheme.accent
                                opacity: 0.15
                                radius: 4
                                z: -1
                            }
                            
                            Row {
                                id: wordRow
                                spacing: 0
                                
                                // Expected characters from target text
                                Repeater {
                                    id: wordCharactersRepeater
                                    model: wordText.length
                                    
                                    Text {
                                        property int charIndex: wordStartIndex + index
                                        property string targetChar: wordText.charAt(index)
                                        property string userInput: typingTest.userInput
                                        property int currentPosition: typingTest.userInput.length
                                        
                                        // Get the actual character user typed at this position
                                        // Word-based: find what user typed in THIS word at THIS position
                                        property string typedChar: {
                                            // Use pre-computed _userWords instead of re-splitting
                                            if (wordIndex >= root._userWords.length) return ""
                                            var userWord = root._userWords[wordIndex]
                                            if (index >= userWord.length) return ""
                                            return userWord.charAt(index)
                                        }
                                        
                                        text: targetChar
                                        font.pixelSize: 24
                                        font.family: "Monospace"
                                        
                                        color: {
                                            // Use pre-computed currentWordIndex instead of re-counting
                                            var currentWordIndex = root.currentWordIndex
                                            
                                            // If this is a future word (not yet reached), always show as untyped
                                            if (wordIndex > currentWordIndex) {
                                                return themeManager.currentTheme.textSecondary
                                            }
                                            
                                            // Check if character was skipped
                                            if (root.skippedIndices.has(charIndex)) {
                                                return Qt.rgba(
                                                    themeManager.currentTheme.textSecondary.r,
                                                    themeManager.currentTheme.textSecondary.g,
                                                    themeManager.currentTheme.textSecondary.b,
                                                    0.3
                                                )
                                            }
                                            
                                            // Use pre-computed _userWords instead of re-splitting
                                            var userWord = wordIndex < root._userWords.length ? root._userWords[wordIndex] : ""
                                            
                                            if (index < userWord.length) {
                                                // Already typed - compare with what was typed
                                                if (typedChar === targetChar) {
                                                    return themeManager.currentTheme.correct
                                                } else {
                                                    return themeManager.currentTheme.incorrect
                                                }
                                            } else if (index === userWord.length && isCurrentWord) {
                                                // Current character - only show accent if this is current word
                                                return themeManager.currentTheme.accent
                                            } else {
                                                // Not typed yet
                                                return themeManager.currentTheme.textSecondary
                                            }
                                        }
                                        
                                        // Cursor indicator for current character - only at EXACT global position
                                        Rectangle {
                                            visible: {
                                                // Must be current word AND at exact character position
                                                // AND position must be within normal word bounds (not in extra chars)
                                                var isVisible = wordIndex === root.currentWordIndex && 
                                                       index === root.currentCharacterIndexInWord &&
                                                       index < wordText.length  // Only show in normal chars, not extra
                                                
                                                return isVisible
                                            }
                                            x: -2
                                            y: 0
                                            width: 2
                                            height: parent.height
                                            color: themeManager.currentTheme.accent
                                            z: 1
                                            
                                            SequentialAnimation on opacity {
                                                running: visible
                                                loops: Animation.Infinite
                                                NumberAnimation { from: 1.0; to: 0.0; duration: 500 }
                                                NumberAnimation { from: 0.0; to: 1.0; duration: 500 }
                                            }
                                        }
                                        
                                        // Underline for incorrect characters
                                        Rectangle {
                                            visible: {
                                                // Use pre-computed currentWordIndex instead of re-counting
                                                if (wordIndex > root.currentWordIndex) return false
                                                
                                                // Use pre-computed _userWords instead of re-splitting
                                                var userWord = wordIndex < root._userWords.length ? root._userWords[wordIndex] : ""
                                                return index < userWord.length && typedChar !== "" && typedChar !== targetChar
                                            }
                                            anchors.bottom: parent.bottom
                                            width: parent.width
                                            height: 2
                                            color: themeManager.currentTheme.incorrect
                                        }
                                    }
                                }
                                
                                // Extra characters typed beyond word length (shown as incorrect)
                                Repeater {
                                    id: extraCharsRepeater
                                    model: {
                                        // Word-based: how many extra chars in this word?
                                        // Use pre-computed _userWords instead of re-splitting
                                        if (wordIndex >= root._userWords.length) return 0
                                        var userWord = root._userWords[wordIndex]
                                        if (userWord.length > wordText.length) {
                                            return userWord.length - wordText.length
                                        }
                                        return 0
                                    }
                                    
                                    Text {
                                        property string userInput: typingTest.userInput
                                        property string extraChar: {
                                            // Use pre-computed _userWords instead of re-splitting
                                            var userWord = wordIndex < root._userWords.length ? root._userWords[wordIndex] : ""
                                            var extraCharPos = wordText.length + index
                                            return extraCharPos < userWord.length ? userWord.charAt(extraCharPos) : ""
                                        }
                                        
                                        text: extraChar
                                        font.pixelSize: 24
                                        font.family: "Monospace"
                                        color: themeManager.currentTheme.incorrect
                                        
                                        // Underline for extra characters
                                        Rectangle {
                                            anchors.bottom: parent.bottom
                                            width: parent.width
                                            height: 2
                                            color: themeManager.currentTheme.incorrect
                                        }
                                    }
                                }
                                
                                // Space after word (except last word)
                                Text {
                                    id: spaceText
                                    visible: wordIndex < wordRepeater.count - 1
                                    text: " "
                                    font.pixelSize: 24
                                    font.family: "Monospace"
                                    
                                    property int spaceCharIndex: wordStartIndex + wordText.length
                                    property string userInput: typingTest.userInput
                                    property int currentPosition: typingTest.userInput.length
                                    
                                    color: {
                                        if (spaceCharIndex < currentPosition) {
                                            if (userInput.charAt(spaceCharIndex) === " ") {
                                                return themeManager.currentTheme.correct
                                            } else {
                                                return themeManager.currentTheme.incorrect
                                            }
                                        } else if (spaceCharIndex === currentPosition) {
                                            return themeManager.currentTheme.accent
                                        } else {
                                            return themeManager.currentTheme.textSecondary
                                        }
                                    }
                                    
                                    // Cursor for space - only when current word is complete
                                    Rectangle {
                                        visible: {
                                            // Show space cursor only when:
                                            // 1. This is the current word
                                            // 2. User has typed all characters in this word
                                            var isVisible = wordIndex === root.currentWordIndex && 
                                                          root.currentCharacterIndexInWord === wordText.length
                                            return isVisible
                                        }
                                        x: -2
                                        y: 0
                                        width: 2
                                        height: parent.height
                                        color: themeManager.currentTheme.accent
                                        z: 1
                                        
                                        SequentialAnimation on opacity {
                                            running: visible
                                            loops: Animation.Infinite
                                            NumberAnimation { from: 1.0; to: 0.0; duration: 500 }
                                            NumberAnimation { from: 0.0; to: 1.0; duration: 500 }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Input field (hidden but functional)
        TextField {
            id: inputField
            width: parent.width
            visible: false
            enabled: root.enabled
            text: typingTest.userInput
            focus: true
            
            onTextChanged: {
                // Avoid feedback loop from Connections block
                if (root._updatingFromCode) return
                
                let oldLength = typingTest.userInput.length
                let newLength = text.length
                
                // Character added (not deleted)
                if (newLength > oldLength) {
                    let newChar = text.charAt(newLength - 1)
                    
                    // Special case for space - check if completed word is correct
                    if (newChar === ' ') {
                        // Use cached _userWords and _targetWords for comparison
                        let userWords = text.split(" ")
                        let targetWords = root._targetWords
                        
                        // The word we just completed is at index (userWords.length - 2)
                        // because the last element after split will be empty string after the space
                        let completedWordIndex = userWords.length - 2
                        
                        if (completedWordIndex >= 0 && completedWordIndex < targetWords.length) {
                            let userWord = userWords[completedWordIndex]
                            let targetWord = targetWords[completedWordIndex]
                            
                            // Check if word length matches
                            if (userWord.length !== targetWord.length) {
                                // Word too long or too short
                                playErrorSound()
                            } else {
                                // Check if all characters match
                                let wordCorrect = true
                                for (let i = 0; i < targetWord.length; i++) {
                                    if (userWord.charAt(i) !== targetWord.charAt(i)) {
                                        wordCorrect = false
                                        break
                                    }
                                }
                                
                                if (wordCorrect) {
                                    playClickSound()
                                } else {
                                    playErrorSound()
                                }
                            }
                        } else {
                            // Space at wrong position
                            playErrorSound()
                        }
                    } else {
                        // Word-based comparison for regular characters
                        // Use cached currentWordIndex instead of recounting spaces
                        let userWords = text.split(" ")
                        let targetWords = root._targetWords
                        let currentWordIndex = root._spacesTyped
                        
                        // Get current word and position within word
                        let userWord = currentWordIndex < userWords.length ? userWords[currentWordIndex] : ""
                        let targetWord = currentWordIndex < targetWords.length ? targetWords[currentWordIndex] : ""
                        let posInWord = userWord.length - 1
                        
                        // Check if it's an error by comparing within the current word
                        let targetChar = posInWord < targetWord.length ? targetWord.charAt(posInWord) : ""
                        
                        if (newChar !== targetChar) {
                            playErrorSound()
                        } else {
                            playClickSound()
                        }
                    }
                } else if (newLength < oldLength) {
                    // Character deleted (backspace)
                    playClickSound()
                }
                
                // Update backend (this will trigger recalcUserInputCache via Connections)
                root._updatingFromCode = true
                typingTest.userInput = text
                root._updatingFromCode = false
                
                // Update scroll immediately on every keystroke
                wordFlow.updateScroll()
            }
            
            // ESC key is now handled in main.qml to open settings
            
            Keys.onTabPressed: {
                if (typingTest.isActive) {
                    restartButton.forceActiveFocus()
                }
            }
            
            Keys.onPressed: (event) => {
                // Handle Space key - just add space, no manipulation
                if (event.key === Qt.Key_Space) {
                    event.accepted = true
                    
                    var userInput = typingTest.userInput
                    
                    // Calculate current word length (chars after last space)
                    var lastSpacePos = userInput.lastIndexOf(' ')
                    var currentWordLength = userInput.length - (lastSpacePos + 1)
                    
                    // Prevent space if no characters typed in current word
                    if (currentWordLength <= 0) {
                        return
                    }
                    
                    // Just add space - that's it!
                    text += " "
                    root._updatingFromCode = true
                    typingTest.userInput = text
                    root._updatingFromCode = false
                    wordFlow.updateScroll()
                    return
                }
                
                // Prevent backspace beyond start
                if (event.key === Qt.Key_Backspace && text.length === 0) {
                    event.accepted = true
                    return
                }
                
                // Prevent backspace to previous completed word (after space)
                if (event.key === Qt.Key_Backspace && text.length > 0) {
                    var lastChar = text.charAt(text.length - 1)
                    if (lastChar === ' ') {
                        event.accepted = true
                        return
                    }
                }
            }
        }
        
        // Instructions with restart button
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 15
            
            Text {
                text: typingTest.isActive ? 
                    qsTr("Press Tab+Enter or ESC to restart") : 
                    qsTr("Start typing to begin the test")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 14
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Button {
                id: restartButton
                visible: typingTest.isActive
                width: 32
                height: 32
                
                Keys.onReturnPressed: {
                    if (typingTest.isActive) {
                        typingTest.StopTest()
                    }
                    typingTest.ResetTest()
                    root.skippedIndices = new Set()
                    inputField.text = ""
                    inputField.forceActiveFocus()
                }
                
                Keys.onEnterPressed: {
                    if (typingTest.isActive) {
                        typingTest.StopTest()
                    }
                    typingTest.ResetTest()
                    root.skippedIndices = new Set()
                    inputField.text = ""
                    inputField.forceActiveFocus()
                }
                
                onClicked: {
                    if (typingTest.isActive) {
                        typingTest.StopTest()
                    }
                    typingTest.ResetTest()
                    root.skippedIndices = new Set()
                    inputField.text = ""
                    inputField.forceActiveFocus()
                }
                
                background: Rectangle {
                    color: parent.pressed ? Qt.darker(themeManager.currentTheme.accent, 1.2) : parent.hovered ? Qt.lighter(themeManager.currentTheme.accent, 1.1) : themeManager.currentTheme.accent
                    radius: 16
                    border.width: parent.activeFocus ? 3 : 0
                    border.color: themeManager.currentTheme.text
                    
                    Behavior on border.width {
                        NumberAnimation { duration: 150 }
                    }
                    
                    scale: parent.activeFocus ? 1.1 : 1.0
                    
                    Behavior on scale {
                        NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
                    }
                }
                
                contentItem: Text {
                    text: "↻"
                    color: themeManager.currentTheme.background
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 20
                    font.bold: true
                }
            }
        }
    }
    
    onVisibleChanged: {
        if (visible) {
            inputField.forceActiveFocus()
        }
    }
}
