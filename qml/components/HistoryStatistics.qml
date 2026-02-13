import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    
    implicitHeight: statsGrid.height
    
    function refresh() {}
    
    GridLayout {
        id: statsGrid
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        columns: root.width > 1000 ? 3 : (root.width > 700 ? 2 : 1)
        rowSpacing: 20
        columnSpacing: 20
        
        HistoryStatsCard {
            title: qsTr("Last 10 Tests")
            limit: 10
        }
        
        HistoryStatsCard {
            title: qsTr("Last 50 Tests")
            limit: 50
        }
        
        HistoryStatsCard {
            title: qsTr("Last 100 Tests")
            limit: 100
        }
    }
}
