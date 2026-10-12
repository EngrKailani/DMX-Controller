const int DMX_TX_PIN = 18;        // Mega TX1
const int DMX_ENABLE_PIN = 2;     // DFR0260 DE control
const int LED_PIN = 13;

byte dmxData[513];

int baseR = 0;
int baseG = 0;
int baseB = 0;
int baseW = 0;

int brightness = 100;

String inputCommand = "";

String activeEffect = "none";
int effectStep = 0;

unsigned long lastEffectTime = 0;
unsigned long demoStartTime = 0;

const int GRADIENT_COLOR_COUNT = 6;
const unsigned long GRADIENT_FADE_TIME = 3000;
const unsigned long DEMO_STEP_TIME = 2500;
const unsigned long DEMO_GRADIENT_TIME = 10000;

byte gradientColors[GRADIENT_COLOR_COUNT][4] = {
  {255, 0, 0, 0},       // Red
  {255, 255, 0, 0},     // Yellow
  {0, 255, 0, 0},       // Green
  {0, 255, 255, 0},     // Cyan
  {0, 0, 255, 0},       // Blue
  {255, 0, 255, 0}      // Magenta
};

void clearDMX() {
  for (int i = 0; i < 513; i++) {
    dmxData[i] = 0;
  }
}

byte scaleValue(int value) {
  if (value <= 0) {
    return 0;
  }

  if (value >= 255 && brightness >= 100) {
    return 255;
  }

  return (byte)((value * brightness) / 100);
}

void updateDMXValues() {
  clearDMX();

  dmxData[0] = 0;
  dmxData[1] = scaleValue(baseR);
  dmxData[2] = scaleValue(baseG);
  dmxData[3] = scaleValue(baseB);
  dmxData[4] = scaleValue(baseW);
}

void sendDMXFrame() {
  Serial1.end();

  pinMode(DMX_TX_PIN, OUTPUT);

  digitalWrite(DMX_TX_PIN, LOW);
  delayMicroseconds(120);

  digitalWrite(DMX_TX_PIN, HIGH);
  delayMicroseconds(16);

  Serial1.begin(250000, SERIAL_8N2);
  Serial1.write(dmxData, 513);
  Serial1.flush();
}

void sendStableFrames(int milliseconds) {
  unsigned long startTime = millis();

  while (millis() - startTime < milliseconds) {
    sendDMXFrame();
    delay(25);
  }
}

void setScene(int r, int g, int b, int w) {
  activeEffect = "none";

  baseR = r;
  baseG = g;
  baseB = b;
  baseW = w;

  updateDMXValues();
  sendStableFrames(300);
}

void setSceneNoStop(int r, int g, int b, int w) {
  baseR = r;
  baseG = g;
  baseB = b;
  baseW = w;

  updateDMXValues();
}

void startGradient() {
  activeEffect = "gradient";
  effectStep = 0;
  lastEffectTime = 0;
}

void startDemo() {
  activeEffect = "demo";
  effectStep = 0;
  lastEffectTime = 0;
  demoStartTime = 0;
}

void handleCommand(String command) {
  command.trim();
  command.toLowerCase();

  if (command == "red") {
    setScene(255, 0, 0, 0);
    Serial.println("Scene: red");
  }
  else if (command == "green") {
    setScene(0, 255, 0, 0);
    Serial.println("Scene: green");
  }
  else if (command == "blue") {
    setScene(0, 0, 255, 0);
    Serial.println("Scene: blue");
  }
  else if (command == "white") {
    setScene(0, 0, 0, 255);
    Serial.println("Scene: white");
  }
  else if (command == "off") {
    setScene(0, 0, 0, 0);
    Serial.println("Scene: off");
  }
  else if (command == "gradient") {
    startGradient();
    Serial.println("Effect: gradient");
  }
  else if (command == "demo") {
    startDemo();
    Serial.println("Effect: demo");
  }
  else if (command.startsWith("brightness")) {
    int spaceIndex = command.indexOf(' ');

    if (spaceIndex > 0) {
      int value = command.substring(spaceIndex + 1).toInt();

      if (value < 0) {
        value = 0;
      }

      if (value > 100) {
        value = 100;
      }

      brightness = value;
      updateDMXValues();
      sendStableFrames(300);

      Serial.print("Brightness: ");
      Serial.println(brightness);
    }
  }
}

void readSerialCommands() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      handleCommand(inputCommand);
      inputCommand = "";
    }
    else {
      inputCommand += c;
    }
  }
}

void runGradient() {
  unsigned long currentTime = millis();

  if (lastEffectTime == 0) {
    lastEffectTime = currentTime;
  }

  unsigned long elapsed = currentTime - lastEffectTime;

  if (elapsed >= GRADIENT_FADE_TIME) {
    effectStep++;

    if (effectStep >= GRADIENT_COLOR_COUNT) {
      effectStep = 0;
    }

    lastEffectTime = currentTime;
    elapsed = 0;
  }

  int nextStep = effectStep + 1;

  if (nextStep >= GRADIENT_COLOR_COUNT) {
    nextStep = 0;
  }

  float progress = elapsed / (float)GRADIENT_FADE_TIME;

  baseR = gradientColors[effectStep][0] +
          (gradientColors[nextStep][0] - gradientColors[effectStep][0]) * progress;

  baseG = gradientColors[effectStep][1] +
          (gradientColors[nextStep][1] - gradientColors[effectStep][1]) * progress;

  baseB = gradientColors[effectStep][2] +
          (gradientColors[nextStep][2] - gradientColors[effectStep][2]) * progress;

  baseW = 0;

  updateDMXValues();
}

void runDemoGradientSegment() {
  unsigned long currentTime = millis();

  if (lastEffectTime == 0) {
    lastEffectTime = currentTime;
  }

  unsigned long elapsed = currentTime - lastEffectTime;

  if (elapsed >= GRADIENT_FADE_TIME) {
    effectStep++;

    if (effectStep >= GRADIENT_COLOR_COUNT) {
      effectStep = 0;
    }

    lastEffectTime = currentTime;
    elapsed = 0;
  }

  int nextStep = effectStep + 1;

  if (nextStep >= GRADIENT_COLOR_COUNT) {
    nextStep = 0;
  }

  float progress = elapsed / (float)GRADIENT_FADE_TIME;

  baseR = gradientColors[effectStep][0] +
          (gradientColors[nextStep][0] - gradientColors[effectStep][0]) * progress;

  baseG = gradientColors[effectStep][1] +
          (gradientColors[nextStep][1] - gradientColors[effectStep][1]) * progress;

  baseB = gradientColors[effectStep][2] +
          (gradientColors[nextStep][2] - gradientColors[effectStep][2]) * progress;

  baseW = 0;

  updateDMXValues();
}

void runDemo() {
  unsigned long currentTime = millis();

  if (demoStartTime == 0) {
    demoStartTime = currentTime;
    lastEffectTime = currentTime;
    effectStep = 0;
  }

  unsigned long demoElapsed = currentTime - demoStartTime;

  if (demoElapsed < DEMO_STEP_TIME) {
    setSceneNoStop(255, 0, 0, 0);
  }
  else if (demoElapsed < DEMO_STEP_TIME * 2) {
    setSceneNoStop(0, 255, 0, 0);
  }
  else if (demoElapsed < DEMO_STEP_TIME * 3) {
    setSceneNoStop(0, 0, 255, 0);
  }
  else if (demoElapsed < DEMO_STEP_TIME * 4) {
    setSceneNoStop(0, 0, 0, 255);
  }
  else if (demoElapsed < (DEMO_STEP_TIME * 4) + DEMO_GRADIENT_TIME) {
    runDemoGradientSegment();
  }
  else {
    setSceneNoStop(0, 0, 0, 0);
    activeEffect = "none";
    Serial.println("Demo complete");
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(DMX_ENABLE_PIN, OUTPUT);

  digitalWrite(DMX_ENABLE_PIN, HIGH);

  Serial.begin(115200);
  Serial1.begin(250000, SERIAL_8N2);

  clearDMX();
  updateDMXValues();

  sendStableFrames(3000);

  Serial.println("LUX DMX Controller Ready");
}

void loop() {
  readSerialCommands();

  if (activeEffect == "gradient") {
    runGradient();
  }
  else if (activeEffect == "demo") {
    runDemo();
  }

  updateDMXValues();
  sendDMXFrame();

  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  delay(25);
}
