import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    
    property int loadedCount: 0
    property int pageSize: 25
    property string dateFilter: "all"
    property string modeFilter: "all"
    property int durationFilter: 0
    
    function refresh() {
        testsModel.clear()
        loadedCount = 0
        loadTests()
    }
    
    function loadTests() {
        var tests = testHistory.GetRecentTests(loadedCount, pageSize, dateFilter, modeFilter, durationFilter)
        
        if (tests.length === 0) {
            return
        }
        
        // Group tests by date and calculate daily averages
        var currentDate = ""
        var dailyTests = []
        
        // Check if we need to merge with the last date in the model
        if (testsModel.count > 0 && loadedCount > 0) {
            // Get the last item's date
            var lastIndex = testsModel.count - 1
            var lastItem = testsModel.get(lastIndex)
            if (!lastItem.isDateSeparator) {
                // Find the last date separator
                for (var k = lastIndex; k >= 0; k--) {
                    var item = testsModel.get(k)
                    if (item.isDateSeparator) {
                        currentDate = new Date(item.timestamp).toLocaleDateString()
                        break
                    }
                }
            }
        }
        
        for (var i = 0; i < tests.length; i++) {
            var test = tests[i]
            var testDate = test.timestamp.toLocaleDateString()
            
            // Add date separator if date changed
            if (testDate !== currentDate) {
                // Calculate average for previous day if exists
                var avgWpm = 0
                var avgAcc = 0
                if (dailyTests.length > 0) {
                    var totalWpm = 0
                    var totalAcc = 0
                    for (var j = 0; j < dailyTests.length; j++) {
                        totalWpm += dailyTests[j].wpm
                        totalAcc += dailyTests[j].accuracy
                    }
                    avgWpm = Math.round(totalWpm / dailyTests.length)
                    avgAcc = totalAcc / dailyTests.length
                }
                
                currentDate = testDate
                var dateLabel = getDateLabel(test.timestamp)
                testsModel.append({
                    "isDateSeparator": true,
                    "dateLabel": dateLabel,
                    "timestamp": test.timestamp,
                    "avgWpm": avgWpm,
                    "avgAccuracy": avgAcc
                })
                dailyTests = []
            }
            
            dailyTests.push(test)
            
            // Add test item
            testsModel.append({
                "isDateSeparator": false,
                "id": test.id,
                "timestamp": test.timestamp,
                "mode": test.mode,
                "duration": test.duration,
                "wordCount": test.wordCount,
                "wpm": test.wpm,
                "accuracy": test.accuracy,
                "correctChars": test.correctChars || 0,
                "incorrectChars": test.incorrectChars || 0
            })
        }
        
        loadedCount += tests.length
    }
    
    function getDateLabel(date) {
        var today = new Date()
        var yesterday = new Date(today)
        yesterday.setDate(yesterday.getDate() - 1)
        
        var testDate = new Date(date)
        testDate.setHours(0, 0, 0, 0)
        today.setHours(0, 0, 0, 0)
        yesterday.setHours(0, 0, 0, 0)
        
        if (testDate.getTime() === today.getTime()) {
            return qsTr("Today")
        } else if (testDate.getTime() === yesterday.getTime()) {
            return qsTr("Yesterday")
        } else {
            return date.toLocaleDateString()
        }
    }
    
    function loadMore() {
        loadTests()
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
        id: testsModel
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        
        // Filter Bar
        HistoryRecentTestsFilter {
            Layout.fillWidth: true
            
            onFiltersChanged: {
                root.dateFilter = selectedDateFilter
                root.modeFilter = selectedModeFilter
                root.durationFilter = selectedDurationFilter
                refresh()
            }
        }
        
        ListView {
            id: testsListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: testsModel
            spacing: 5
            clip: true
            
            delegate: Loader {
                width: ListView.view.width
                
                sourceComponent: model.isDateSeparator ? dateSeparatorComponent : testItemComponent
                
                onLoaded: {
                    if (item) {
                        if (model.isDateSeparator) {
                            item.dateText = model.dateLabel || ""
                            item.dailyAvgWpm = model.avgWpm || 0
                            item.dailyAvgAcc = model.avgAccuracy || 0
                        } else {
                            item.testTimestamp = model.timestamp
                            item.testId = model.id || 0
                            item.mode = model.mode || ""
                            item.testDuration = model.duration || 0
                            item.words = model.wordCount || 0
                            item.wpmValue = model.wpm || 0
                            item.accuracyValue = model.accuracy || 0
                            item.correct = model.correctChars || 0
                            item.incorrect = model.incorrectChars || 0
                        }
                    }
                }
            }
        }
        
        // Load More Button
        Button {
            id: loadMoreBtn
            visible: loadedCount < testHistory.totalTests && testsModel.count > 0
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            
            text: {
                var remaining = testHistory.totalTests - loadedCount
                return qsTr("Load More") + " (" + Math.min(25, remaining) + " " + qsTr("more") + ")"
            }
            
            onClicked: loadMore()
            
            background: Rectangle {
                color: parent.pressed ? Qt.darker(themeManager.currentTheme.surface, 1.2) : themeManager.currentTheme.surface
                radius: 8
                border.width: 1
                border.color: parent.hovered ? themeManager.currentTheme.accent : themeManager.currentTheme.border
                Behavior on border.color {
                    ColorAnimation { duration: 150 }
                }
            }
            
            contentItem: Text {
                text: parent.text
                font.pixelSize: 14
                color: themeManager.currentTheme.text
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
    
    Component {
        id: dateSeparatorComponent
        HistoryRecentTestDateSeparator {}
    }
    
    Component {
        id: testItemComponent
        HistoryRecentTestItem {
            onDeleteClicked: function(id) {
                testHistory.DeleteTest(id)
                refresh()
            }
        }
    }
}
