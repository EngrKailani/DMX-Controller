# DMX Controller

DMX Controller is a local web-based RGBW lighting controller. It uses a Flask web interface to send scene and brightness commands to an Arduino over serial. The Arduino then outputs DMX data to control an RGBW lighting fixture.

## Project Overview

This project was built for a LUX Dynamics DMX lighting demonstration. The web app provides large scene buttons for common lighting outputs such as red, green, blue, white, gradient, off, and demo sequence. It also includes a dimmer slider and quick brightness buttons.

The system has two main parts:

1. A Python Flask web app that runs on the computer
2. An Arduino sketch that receives serial commands and sends DMX frames

## Features

- Local browser-based lighting controller
- RGBW scene buttons
- Brightness control from 0% to 100%
- Quick dimmer buttons for 1%, 10%, and full brightness
- Gradient effect
- Demo sequence
- Arduino serial communication
- Direct DMX frame generation from the Arduino
- Responsive web interface for desktop or tablet use
- LUX Dynamics branded interface

## Supported Scenes

| Button | Command Sent to Arduino | DMX Output |
|---|---|---|
| Red | `red` | Red channel on |
| Green | `green` | Green channel on |
| Blue | `blue` | Blue channel on |
| White | `white` | White channel on |
| Gradient | `gradient` | Cycles through multiple colors |
| Off | `off` | All channels off |
| Demo Sequence | `demo` | Runs red, green, blue, white, gradient, then off |

## Brightness Controls

The interface includes a dimmer slider from 0% to 100%.

It also includes shortcut buttons:

| Button | Brightness Command |
|---|---|
| 1% | `brightness 1` |
| 10% | `brightness 10` |
| Full | `brightness 100` |

Brightness is applied in the Arduino code by scaling the RGBW channel values before sending the DMX frame.

## Project Files

| File | Purpose |
|---|---|
| `app.py` | Flask server that serves the web interface and sends serial commands to the Arduino |
| `templates/index.html` | Main web page for the DMX controller interface |
| `static/style.css` | Styling for the web interface |
| `static/lux_logo_white.png` | LUX Dynamics logo used in the interface |
| `LUX_DMX_Controller_Working/LUX_DMX_Controller_Working.ino` | Arduino sketch that receives commands and sends DMX frames |
| `find_mouse.py` | Utility script that prints mouse coordinates every 2 seconds |
| `list_windows.py` | Utility script that lists open window titles |
| `README.md` | Project documentation |

## Requirements

### Computer Requirements

- Python 3
- Flask
- PySerial
- A browser such as Chrome, Edge, or Firefox
- USB connection to the Arduino

### Optional Python Packages

These are only needed for the utility scripts:

- PyAutoGUI
- PyGetWindow

### Hardware Requirements

- Arduino Mega or compatible board with `Serial1`
- DMX output circuit, shield, or adapter
- RGBW DMX fixture
- USB cable for Arduino serial communication
- DMX cable

The Arduino sketch uses:

```text
DMX TX Pin: 18
DMX Enable Pin: 2
Status LED Pin: 13
```

Pin 18 is used because it is the TX1 pin on an Arduino Mega.

## DMX Channel Mapping

The Arduino code sends RGBW data starting at DMX address 1.

| DMX Channel | Function |
|---|---|
| Channel 1 | Red |
| Channel 2 | Green |
| Channel 3 | Blue |
| Channel 4 | White |

The DMX data array uses index `0` for the DMX start code. The RGBW values are stored in indexes `1` through `4`.

## Standard Output Profile

The web interface displays this output profile:

| Color Channel | Current |
|---|---:|
| Red | 640 mA |
| Green | 450 mA |
| Blue | 490 mA |
| White | 1050 mA max |

## Setup Instructions

### 1. Install Python Dependencies

Open Command Prompt or PowerShell in the project folder.

Install the required Python packages:

```bash
pip install flask pyserial
```

If you want to use the optional utility scripts, install:

```bash
pip install pyautogui pygetwindow
```

### 2. Check the Arduino Serial Port

The Flask app defaults to **COM5** at **115200 baud**. You can select the USB serial port using `DMX_ARDUINO_PORT` without editing the source. In Windows PowerShell:

```powershell
$env:DMX_ARDUINO_PORT = "COM5"   # Change to your actual COM port
python app.py
```

If the Arduino is connected on another port, select that one instead.

To find the correct port, open Device Manager on Windows and check the Arduino port under **Ports (COM & LPT)**.

### 3. Upload the Arduino Sketch

Open this file in the Arduino IDE:

```text
LUX_DMX_Controller_Working/LUX_DMX_Controller_Working.ino
```

Select the correct board and port.

Upload the sketch to the Arduino.

**Recovery correction:** The tracked Arduino sketch now starts with `const int DMX_TX_PIN = 18;`. The original `fconst` typo was corrected directly on `main`. The sketch has not been recompiled or tested against actual DMX hardware in this recovery pass.

### 4. Connect the DMX Hardware

Connect the Arduino DMX output circuit or shield to the DMX fixture.

Make sure the RGBW fixture is addressed so that:

```text
Address 1 = Red
Address 2 = Green
Address 3 = Blue
Address 4 = White
```

### 5. Run the Flask App

From the project folder, run:

```bash
python app.py
```

The app runs on port `5000`.

Open this address in a browser:

```text
http://localhost:5000
```

**Security:** The server now listens on `127.0.0.1` only by default. It cannot be reached by another device on the LAN unless an operator intentionally changes the host setting.

For an isolated, trusted test LAN only, the operator can explicitly enable network access:

```powershell
$env:DMX_HOST = "0.0.0.0"
python app.py
```

Then browse to `http://<computer-ip-address>:5000` from an authorized device. **The app has no HTTP authentication**, so do not enable this mode on a shared/untrusted network or expose port 5000 to the internet. Clear `DMX_HOST` or set it to `127.0.0.1` to return to local-only operation.

## How to Use the Controller

1. Connect the Arduino to the computer.
2. Connect the Arduino DMX output to the RGBW fixture.
3. Upload the Arduino sketch.
4. Run the Flask app with `python app.py`.
5. Open `http://localhost:5000` in a browser.
6. Click a scene button to trigger a lighting scene.
7. Use the dimmer slider to change brightness.
8. Use the 1%, 10%, and Full buttons for quick brightness changes.

## Web Routes

### Home Page

```text
/
```

Displays the DMX controller web interface.

### Scene Control

```text
/scene/<scene_name>
```

Method:

```text
POST
```

Valid scene names:

```text
red
green
blue
white
gradient
off
demo
```

Example:

```text
/scene/red
```

A successful response returns JSON:

```json
{
  "status": "ok",
  "scene": "red"
}
```

An invalid scene returns:

```json
{
  "status": "error",
  "message": "Invalid scene"
}
```

### Brightness Control

```text
/brightness/<value>
```

Method:

```text
POST
```

Valid brightness range:

```text
0 to 100
```

Example:

```text
/brightness/50
```

A successful response returns JSON:

```json
{
  "status": "ok",
  "brightness": 50
}
```

An invalid brightness value returns:

```json
{
  "status": "error",
  "message": "Brightness must be between 0 and 100"
}
```

## Arduino Serial Commands

The Flask app sends text commands to the Arduino over serial.

Scene commands:

```text
red
green
blue
white
gradient
off
demo
```

Brightness command format:

```text
brightness <value>
```

Examples:

```text
brightness 1
brightness 10
brightness 100
```

Each command is sent with a newline character so the Arduino can detect the end of the command.

## How the Arduino Code Works

The Arduino listens for serial commands from the computer at:

```text
115200 baud
```

When a complete command is received, the Arduino checks the command and updates the active lighting scene.

For fixed color scenes, the Arduino sets the base RGBW values and sends stable DMX frames.

For example:

```text
red = 255, 0, 0, 0
green = 0, 255, 0, 0
blue = 0, 0, 255, 0
white = 0, 0, 0, 255
off = 0, 0, 0, 0
```

The brightness value scales the output before it is sent through DMX.

The Arduino continuously sends DMX frames in the main loop. This keeps the fixture updated and prevents the DMX output from dropping.

## Gradient Effect

The gradient effect cycles through these colors:

```text
Red
Yellow
Green
Cyan
Blue
Magenta
```

The fade time between gradient colors is:

```text
3000 milliseconds
```

The gradient loops until another scene command is received.

## Demo Sequence

The demo sequence runs through:

```text
Red
Green
Blue
White
Gradient
Off
```

The fixed color steps last:

```text
2500 milliseconds each
```

The gradient portion lasts:

```text
10000 milliseconds
```

After the sequence completes, the fixture turns off and the demo stops.

## Utility Scripts

### `find_mouse.py`

This script prints the current mouse position every 2 seconds.

Run it with:

```bash
python find_mouse.py
```

This is useful when recording screen coordinates for automation testing.

### `list_windows.py`

This script lists open window titles.

Run it with:

```bash
python list_windows.py
```

This is useful when checking what application windows are visible to Python automation tools.

## Troubleshooting

### The web page does not open

Make sure the Flask app is running.

Run:

```bash
python app.py
```

Then open:

```text
http://localhost:5000
```

### Flask is not installed

Install Flask:

```bash
pip install flask
```

### PySerial is not installed

Install PySerial:

```bash
pip install pyserial
```

### Arduino command failed

Check these items:

- The Arduino is connected by USB
- The COM port in `app.py` matches the Arduino COM port
- The Arduino sketch has been uploaded
- No other program is using the Arduino serial port
- The baud rate is set to `115200`

### The Arduino does not upload

Check that the correct board and COM port are selected in the Arduino IDE.

If the sketch does not compile, check the first line of the `.ino` file. It should be:

```cpp
const int DMX_TX_PIN = 18;
```

### The fixture does not respond

Check these items:

- The DMX output hardware is connected correctly
- The DMX fixture is powered on
- The fixture is set to the correct starting address
- The fixture uses RGBW channels on DMX addresses 1 through 4
- The DMX cable is connected to the correct input
- The Arduino is sending data through the correct TX pin

### Brightness changes but colors look wrong

Confirm that the fixture channel order is RGBW.

This project assumes:

```text
Channel 1 = Red
Channel 2 = Green
Channel 3 = Blue
Channel 4 = White
```

Some fixtures use a different channel order.

### The app works locally but not from another device

The Flask app now starts with:

```python
app.run(host=os.environ.get("DMX_HOST", "127.0.0.1"), port=5000, debug=False)
```

The default is **loopback-only**, so LAN access requires explicitly setting `DMX_HOST`. This does not add HTTP authentication.

If another device cannot connect, check:

- Both devices are on the same network
- The host computer firewall allows port `5000`
- The correct local IP address is being used

## Development Notes

The Flask app uses these Python modules:

```python
from flask import Flask, render_template, jsonify
import serial
import time
```

The app connects to the Arduino only when needed. If the Arduino is already connected and the serial port is open, it reuses the existing connection.

The valid scenes are defined in `app.py`:

```python
valid_scenes = ["red", "green", "blue", "white", "gradient", "off", "demo"]
```

The default Arduino settings are:

```python
ARDUINO_PORT = "COM5"
ARDUINO_BAUD = 115200
```

The web interface sends commands using JavaScript `fetch()` requests.

## Safety Notes

This project is for demo mode only. The interface triggers preset DMX scenes. It does not change driver current settings, DMX addresses, or CO programming.

Before connecting lighting hardware, confirm that the fixture, driver, and DMX wiring are configured correctly.

## Credits

Developed by Kailani Puava Alarcon.

Project created for LUX Dynamics.

## License

No license has been specified for this project.

## October 2026 Engineering Recovery and Limitations

The recovered firmware formerly began with `fconst int DMX_TX_PIN`, a clear compile error. This typo was corrected. The Flask app also now **binds to localhost by default**, and the Arduino serial port is configurable via `DMX_ARDUINO_PORT`. The original scene commands, RGBW mapping, demo sequence, and hardware pin constants remain intact.

The server exposes unauthenticated POST endpoints, so the user interface's buttons do not constitute access control. **Do not expose port 5000 on an untrusted network.** For any deployed device, add authentication/authorization, CSRF/origin protections, rate limiting, command logging and fixture-safety controls before allowing remote access. The `status: ok` HTTP response means the Python process wrote a command to its serial port; it is **not** an acknowledgement from the fixture or a verified DMX physical response.

The HTML page lists nominal color-channel currents (640/450/490 mA and up to 1050 mA). These are display text only; the firmware controls **0–255 DMX byte values** and contains no current-measurement feedback or current-programming code. Do not infer actual LED current from the dimmer percentage without the fixture's DMX personality, driver specifications and measurements.

The firmware sends a 513-slot DMX packet at 250000 baud with two stop bits per slot, a 120 µs BREAK, and a 16 µs mark-after-break. Ideal serialization is approximately 22.708 ms. The main loop also waits 25 ms, implying roughly 21 frames per second **before additional processing**, not a verified refresh-rate measurement.

[Source-based engineering recovery, reconstructed diagrams and independent offline tests](https://github.com/EngrKailani/engrkailani.github.io/tree/main/projects/dmx). Original Arduino and fixture calibration data were not recovered in this documentation pass.
