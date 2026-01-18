#include <Bluepad32.h>

const int IN1 = 17; 
const int IN2 = 16; 
const int IN3 = 5;  
const int IN4 = 18;

ControllerPtr myControllers[BP32_MAX_GAMEPADS] = {};

void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (!myControllers[i]) {
      myControllers[i] = ctl;
      Serial.printf("Controller connected at index %d\n", i);
      return;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      return;
    }
  }
}

void processGamepad(ControllerPtr ctl) {
  int16_t axisX = ctl->axisX();
  int16_t axisY = ctl->axisRY();
  const int deadzone = 75;

  if (abs(axisX) < deadzone && abs(axisY) < deadzone) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    return;
  }

  bool forward = axisY < -100;
  bool backward = axisY > 100;
  bool left = axisX < -100;
  bool right = axisX > 100;

  digitalWrite(IN1, forward || left ? HIGH : LOW);
  digitalWrite(IN2, backward || right ? HIGH : LOW);
  digitalWrite(IN3, forward || right ? HIGH : LOW);
  digitalWrite(IN4, backward || left ? HIGH : LOW);
}

void processControllers() {
  for (auto ctl : myControllers) {
    if (ctl && ctl->isConnected() && ctl->hasData() && ctl->isGamepad()) {
      processGamepad(ctl);
    }
  }
}

void setup() {
  Serial.begin(115200);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  if (BP32.update()) {
    processControllers();
  }
}
