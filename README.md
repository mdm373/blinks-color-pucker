# Color Pucker

> A color selection utility for [blinks](https://blinks.games/)

| Number of Blinks | Number of Players | Duration             | Recommended Ages |
|:----------------:|:-----------------:|:--------------------:|:----------------:|
| 4                | 1                 | ¯\\_ (ツ)_/¯         | ¯\\_ (ツ)_/¯      |

## Install

Upload via one of the following methods...

- Build and upload from source (open `./main` in ArduinoIDE)

## 📝 Directions

Join blinks into a cluster of four so that one blink is touching three other blinks on consecutive faces. Double tap one of the blinks that shares three neighbors to make it the `color puck` and its neighbors the RGB `component pucks`.

The `color puck` displays the RGB (Red/Green/Blue) color formed by its three color `component puck` neighbors. Each component value ranges from 0  to 31, the maximum spectrum supported by blinks. Tapping, double tapping or triple tapping a `component puck` will increase the component's value by 1, 5 and 10 respectively (wrapping back to 0 after 31). Long pressing a component puck will display its numeric value in base six in white (see notes below). Long pressing it again will return it to it color component brightness.

### Interpreting The Base Six Display

Blinks color values have a range of 0-31. Luckily, blinks have six individual faces and two digits in base six gets us up to 41. So, to show their color value, components will alternate between showing a bright number of lit faces and a dim number of lit faces where the bright value is the most significant digit and the dimmer value the least significant.

> Ex: Long pressing a blue `component puck` and having it blink between 3 bright faces and 2 dim faces = 3*6 + 2 = 20. That `component puck` represents a blue component of value 20 in base 10.

Reading each `component puck` will give us the value of the displayed `color puck` on a component scale of 0-31 for red, green and blue, contently, this matches the `MAKECOLOR_5BIT_RGB` color constructor provided in `blinklib.h`

## Develop

### Requirements

- [ArduinoIDE](https://www.arduino.cc/en/Guide/HomePage)
- [BlinksSDK](https://github.com/Move38/Blinks-SDK)

### Optionals

- VS Code w/ Microsoft [Arduino plugin](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino)
