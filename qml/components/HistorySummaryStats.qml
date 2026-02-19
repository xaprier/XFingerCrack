import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    implicitHeight: overviewGrid.implicitHeight + 40
    color: themeManager.currentTheme.surface
    radius: 12
    border.width: 2
    border.color: themeManager.currentTheme.border
    
    GridLayout {
        id: overviewGrid
        anchors.fill: parent
        anchors.margins: 20
        columns: root.width < 400 ? 1 : (root.width < 700 ? 2 : (root.width < 1000 ? 3 : 4))
        rowSpacing: 20
        columnSpacing: 20
        
        HistoryStatCard {
            title: qsTr("Total Tests")
            value: testHistory.totalTests.toString()
            icon: "📊"
        }
        
        HistoryStatCard {
            title: qsTr("Current Streak")
            value: testHistory.currentStreak.toString() + " " + qsTr("days")
            icon: "🔥"
        }
        
        HistoryStatCard {
            title: qsTr("Longest Streak")
            value: testHistory.longestStreak.toString() + " " + qsTr("days")
            icon: "🏆"
        }
        
        HistoryStatCard {
            title: qsTr("Highest WPM")
            value: testHistory.highestWpm.toString()
            icon: "⚡"
        }
        
        HistoryStatCard {
            title: qsTr("Average WPM")
            value: Math.round(testHistory.averageWpm).toString()
            icon: "📈"
        }
        
        HistoryStatCard {
            title: qsTr("Average Accuracy")
            value: testHistory.averageAccuracy.toFixed(1) + "%"
            icon: "🎯"
        }
        
        HistoryStatCard {
            title: qsTr("Total Time")
            value: {
                var totalMinutes = Math.floor(testHistory.totalTests * 60 / 60)
                var hours = Math.floor(totalMinutes / 60)
                var mins = totalMinutes % 60
                return hours + "h " + mins + "m"
            }
            icon: "⏱️"
        }
        
        HistoryStatCard {
            title: qsTr("Tests Today")
            value: testHistory.GetTestsByDate(new Date()).length.toString()
            icon: "📅"
        }
    }
}
