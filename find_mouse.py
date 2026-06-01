import pyautogui
import time

print("Move your mouse over each ESA2 scene button.")
print("Coordinates will print every 2 seconds.")
print("Press CTRL+C to stop.")

while True:
    x, y = pyautogui.position()
    print(f"x={x}, y={y}")
    time.sleep(2)