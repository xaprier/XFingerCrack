import QtQuick
import QtQuick.Controls
import QtCharts

Rectangle {
    id: root
    
    property var testResult
    
    width: parent.width
    height: 300
    color: themeManager.currentTheme.surface
    radius: 15
    
    // Populate chart when testResult changes
    onTestResultChanged: {
        // Always clear first to avoid accumulating old data
        wpmSeries.clear()
        
        if (!testResult || !testResult.wpmHistory)
            return
        
        var history = testResult.wpmHistory
        
        for (var i = 0; i < history.length; ++i) {
            var p = history[i]
            if (p && p.timestamp !== undefined && p.wpm !== undefined) {
                wpmSeries.append(p.timestamp / 1000.0, p.wpm)
            }
        }
        
        // Force update axis ranges after populating data
        xAxis.max = testResult && testResult.statistics
                    ? Math.max(10, testResult.statistics.totalTime / 1000)
                    : 10
        yAxis.max = testResult
                    ? Math.max(50, testResult.GetMaxWpm() + 10)
                    : 50
    }
    
    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10
        
        TextWithFont {
            text: qsTr("WPM Over Time")
            color: themeManager.currentTheme.textSecondary
            font.pixelSize: 18
            font.bold: true
        }
        
        ChartView {
            id: chartView
            width: parent.width
            height: parent.height - 30
            antialiasing: true
            backgroundColor: themeManager.currentTheme.surface
            plotAreaColor: themeManager.currentTheme.background
            legend.visible: false
            theme: ChartView.ChartThemeLight
            
            Component.onCompleted: {
                // Set chart theme based on current theme
                // ChartThemeLight works better with custom colors
            }
            
            // Tooltip for hover
            ToolTip {
                id: chartTooltip
                visible: false
                
                background: Rectangle {
                    color: themeManager.currentTheme.surface
                    border.color: themeManager.currentTheme.border
                    border.width: 1
                    radius: 4
                }
                
                contentItem: TextWithFont {
                    text: chartTooltip.text
                    color: themeManager.currentTheme.textSecondary
                    font.pixelSize: 12
                }
            }
            
            ValueAxis {
                id: xAxis
                min: 0
                max: testResult && testResult.statistics ? Math.max(10, testResult.statistics.totalTime / 1000) : 10
                tickCount: 6
                labelFormat: "%d"
                titleText: ""
                gridVisible: true
                lineVisible: true
                labelsVisible: true
                labelsColor: themeManager.currentTheme.textSecondary
            }
            
            ValueAxis {
                id: yAxis
                min: 0
                max: testResult ? Math.max(50, testResult.GetMaxWpm() + 10) : 50
                tickCount: 6
                labelFormat: "%d"
                titleText: ""
                gridVisible: true
                lineVisible: true
                labelsVisible: true
                labelsColor: themeManager.currentTheme.textSecondary
            }
            
            LineSeries {
                id: wpmSeries
                axisX: xAxis
                axisY: yAxis
                color: themeManager.currentTheme.accent
                width: 3
                
                onHovered: function(point, state) {
                    if (state) {
                        chartTooltip.text = "Time: " + point.x.toFixed(1) + "s | WPM: " + point.y.toFixed(0)
                        
                        // Position tooltip near the point
                        var pos = chartView.mapToPosition(point, wpmSeries)
                        chartTooltip.x = pos.x + 10
                        chartTooltip.y = pos.y - 30
                        chartTooltip.visible = true
                    } else {
                        chartTooltip.visible = false
                    }
                }
            }
            
            // Custom axis titles with theme colors
            TextWithFont {
                text: qsTr("Time (seconds)")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                font.bold: true
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 5
            }
            
            TextWithFont {
                text: qsTr("WPM")
                color: themeManager.currentTheme.textSecondary
                font.pixelSize: 12
                font.bold: true
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 5
                rotation: -90
            }
        }
    }
}
