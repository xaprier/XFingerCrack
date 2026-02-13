import QtQuick

QtObject {
    id: responsive
    
    // Screen size properties
    property real screenWidth: 1200
    property real screenHeight: 800
    
    // Device type detection
    readonly property bool isMobile: screenWidth < 768
    readonly property bool isTablet: screenWidth >= 768 && screenWidth < 1024
    readonly property bool isDesktop: screenWidth >= 1024
    
    // Orientation
    readonly property bool isPortrait: screenHeight > screenWidth
    readonly property bool isLandscape: screenWidth > screenHeight
    
    // Adaptive scaling factor based on screen width
    readonly property real scaleFactor: Math.max(0.7, Math.min(1.5, screenWidth / 1200))
    
    // Base sizes (will be multiplied by scaleFactor)
    readonly property int baseFontSize: 16
    readonly property int baseTitleSize: 24
    readonly property int baseHeaderSize: 32
    readonly property int baseSpacing: 10
    readonly property int basePadding: 20
    readonly property int baseButtonHeight: 40
    
    // Scaled sizes
    readonly property int fontSize: Math.round(baseFontSize * scaleFactor)
    readonly property int titleSize: Math.round(baseTitleSize * scaleFactor)
    readonly property int headerSize: Math.round(baseHeaderSize * scaleFactor)
    readonly property int spacing: Math.round(baseSpacing * scaleFactor)
    readonly property int padding: Math.round(basePadding * scaleFactor)
    readonly property int buttonHeight: Math.round(baseButtonHeight * scaleFactor)
    
    // Touch-specific sizes (minimum touch target: 44x44 points)
    readonly property int minTouchSize: isMobile ? 44 : 32
    readonly property int touchPadding: isMobile ? 16 : 8
    
    // Content width constraints
    readonly property int maxContentWidth: isMobile ? screenWidth - 40 : 
                                           isTablet ? 700 : 1000
    
    // Typing area font size (bigger for typing)
    readonly property int typingFontSize: isMobile ? 18 : 24
    
    // Word spacing
    readonly property int wordSpacing: isMobile ? 6 : 8
    
    // Function to update screen size
    function updateSize(width, height) {
        screenWidth = width
        screenHeight = height
    }
}
