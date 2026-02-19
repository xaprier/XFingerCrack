import QtQuick

/**
 * @brief Text component with automatic font management
 * 
 * This component extends the standard Text element and automatically
 * applies the current font from FontManager. All standard Text properties
 * are preserved and can be used normally.
 * 
 */
Text {
    font.family: fontManager.currentFont
}