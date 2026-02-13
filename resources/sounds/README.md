# Sound Effects

This directory contains sound effects for typing feedback.

## Source

All sound files are sourced from the [Monkeytype project](https://github.com/monkeytypegame/monkeytype/tree/master/frontend/static/sound).

## Structure

### Click Sounds
Different mechanical keyboard sound profiles:
- `click4/` - Mechanical keyboard sound variant 4
- `click5/` - Mechanical keyboard sound variant 5
- `click6/` - Mechanical keyboard sound variant 6
- `click15/` - Mechanical keyboard sound variant 15

Each click sound directory contains multiple variations (e.g., `click4_1.wav`, `click4_2.wav`, etc.) that are randomly selected during typing to create a more natural and varied sound experience.

### Error Sounds
- `error2/` - Error notification sound played when a typing mistake is made

## Usage

Sounds are played based on user settings:
- **Sound Enabled**: Master toggle for all sounds
- **Click Sound**: Played on every keystroke when enabled
- **Error Sound**: Played when a typing error occurs

The application randomly selects from available sound variations within the chosen click sound profile to provide a more realistic typing experience.

## License

These sound files are part of the Monkeytype project and are subject to their license terms.
