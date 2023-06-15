# Code Readme

This repository contains the code for a temperature and humidity control system using an Arduino and 2x DHT22 sensors. The system utilizes a TFT LCD display for user interface and interaction.

## Dependencies
The following libraries are required to run the code:
- Elegoo_GFX
- Elegoo_TFTLCD
- TouchScreen
- DHT

## Pin Configuration
The code assumes the following pin configuration:

### TFT LCD
- LCD_CS: A3
- LCD_CD: A2
- LCD_WR: A1
- LCD_RD: A0
- LCD_RESET: A4

### TouchScreen
- YP: A3
- XM: A2
- YM: 9
- XP: 8

### Sensors (DHT22)
- DHT1_PIN: 31 (Top sensor)
- DHT2_PIN: 33 (Bottom sensor)

### Relays
- Fan: 49
- Heat: 51
- Move: 53

Please make sure to adjust the pin configuration in the code if your setup differs.

## Color Configuration
The code includes predefined color constants for various elements on the TFT display. You can modify these constants to customize the colors to your preference.

- BACKG_Color: Background color for the display
- EDITABLE_Color: Color for user-editable values
- NONEDIT_Color: Color for non-editable values
- DECORATON_Color: Color for decorations
- WARNING_Color: Color for error messages

## Classes
### Relais
The `Relais` class represents a relay module connected to the Arduino. It provides methods to initialize, turn on, and turn off the relay.

### TouchButton
The `TouchButton` class represents a touch-sensitive button on the TFT display. It provides a method to check if the button is pressed based on the provided coordinates.

## Functions
### setup()
The `setup()` function is called once at the beginning of the program. It initializes the relays, TFT display, and builds the graphical user interface (GUI). It also sets initial values for temperature, humidity, and target temperature, and checks the system's start/stop status.

### build_gui()
The `build_gui()` function configures the TFT display and draws the static elements of the GUI.

### update_temp()
The `update_temp()` function updates the displayed temperature value on the TFT display.

### update_hum()
The `update_hum()` function updates the displayed humidity value on the TFT display.

### update_target()
The `update_target()` function updates the displayed target temperature value on the TFT display.

### check_started()
The `check_started()` function updates the display to indicate the current start/stop status of the system.

### show_error()
The `show_error()` function displays an error message on the TFT display.

### loop()
The `loop()` function is the main program loop that continuously runs. It handles sensor readings, temperature and humidity control, touch input, and updating the display.
