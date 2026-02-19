import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    
    function refresh() {
        timeBasedModel.clear()
        wordsBasedModel.clear()
        
        var timeTests = testHistory.GetBestTests("time", 0)
        for (var i = 0; i < timeTests.length; i++) {
            timeBasedModel.append(timeTests[i])
        }
        
        var wordsTests = testHistory.GetBestTests("words", 0)
        for (var j = 0; j < wordsTests.length; j++) {
            wordsBasedModel.append(wordsTests[j])
        }
    }
    
    Component.onCompleted: {
        refresh()
    }
    
    Connections {
        target: testHistory
        function onHistoryChanged() {
            refresh()
        }
    }
    
    ListModel {
        id: timeBasedModel
    }
    
    ListModel {
        id: wordsBasedModel
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        
        TextWithFont { 
            text: qsTr("Best Results")
            font.pixelSize: 20
            font.bold: true
            color: themeManager.currentTheme.text
        }
        
        // Mode TabBar
        TabBar {
            id: modeTabBar
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            
            background: Rectangle {
                color: "transparent"
            }
            
            TabButton {
                text: qsTr("Time-Based")
                background: Rectangle {
                    color: parent.checked ? themeManager.currentTheme.surface : "transparent"
                    radius: 6
                    border.width: 1
                    border.color: parent.checked ? themeManager.currentTheme.border : "transparent"
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
                text: qsTr("Words-Based")
                background: Rectangle {
                    color: parent.checked ? themeManager.currentTheme.surface : "transparent"
                    radius: 6
                    border.width: 1
                    border.color: parent.checked ? themeManager.currentTheme.border : "transparent"
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
            currentIndex: modeTabBar.currentIndex
            
            // Time-Based Tests
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: timeBasedModel
                spacing: 5
                clip: true
                
                delegate: HistoryBestResultItem {
                    width: ListView.view.width
                    rank: index + 1
                    duration: model.duration
                    wpm: model.wpm
                    accuracy: model.accuracy
                    timestamp: model.timestamp
                    isTimeBased: true
                }
            }
            
            // Words-Based Tests
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: wordsBasedModel
                spacing: 5
                clip: true
                
                delegate: HistoryBestResultItem {
                    width: ListView.view.width
                    rank: index + 1
                    wordCount: model.wordCount
                    wpm: model.wpm
                    accuracy: model.accuracy
                    timestamp: model.timestamp
                    isTimeBased: false
                }
            }
        }
    }
}
