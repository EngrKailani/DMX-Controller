from flask import Flask, render_template, jsonify
import serial
import time
import os

app = Flask(__name__)

ARDUINO_PORT = os.environ.get("DMX_ARDUINO_PORT", "COM5")
ARDUINO_BAUD = 115200

arduino = None


def connect_arduino():
    global arduino

    if arduino is not None and arduino.is_open:
        return True

    try:
        arduino = serial.Serial(ARDUINO_PORT, ARDUINO_BAUD, timeout=1)
        time.sleep(2)
        print("Arduino connected")
        return True
    except Exception as e:
        print(f"Arduino connection failed: {e}")
        return False


def send_to_arduino(command):
    if not connect_arduino():
        return False

    try:
        arduino.write((command + "\n").encode())
        print(f"Sent to Arduino: {command}")
        return True
    except Exception as e:
        print(f"Failed to send command: {e}")
        return False


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/scene/<scene_name>", methods=["POST"])
def scene(scene_name):
    valid_scenes = ["red", "green", "blue", "white", "gradient", "off", "demo"]

    if scene_name not in valid_scenes:
        return jsonify({"status": "error", "message": "Invalid scene"}), 400

    success = send_to_arduino(scene_name)

    if not success:
        return jsonify({"status": "error", "message": "Arduino command failed"}), 500

    return jsonify({"status": "ok", "scene": scene_name})


@app.route("/brightness/<int:value>", methods=["POST"])
def brightness(value):
    if value < 0 or value > 100:
        return jsonify({"status": "error", "message": "Brightness must be between 0 and 100"}), 400

    success = send_to_arduino(f"brightness {value}")

    if not success:
        return jsonify({"status": "error", "message": "Brightness command failed"}), 500

    return jsonify({"status": "ok", "brightness": value})


if __name__ == "__main__":
    connect_arduino()
    app.run(host=os.environ.get("DMX_HOST", "127.0.0.1"), port=5000, debug=False)