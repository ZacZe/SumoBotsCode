#include <Bluepad32.h>

#define IN1 27 // Motor 1 forward
#define IN2 14  // Motor 1 reverse
#define IN3 12  // Motor 2 reverse
#define IN4 13  // Motor 2 forward

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);

            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n",
                          ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);

            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }

    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not find empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void processGamepad(ControllerPtr ctl) {
    int16_t axisX = ctl->axisX();    // Left joystick X-axis
    int16_t axisY = ctl->axisRY();   // Right joystick Y-axis

    // First, check for braking (joystick centered)
    if (abs(axisX) < 50 && abs(axisY) < 50) {
        // Apply braking: set both inputs of each motor to HIGH to short-circuit the windings
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        return; // No movement if the joystick is in the neutral position
    }

    // If not braking, ensure motors are stopped first
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    // Handle motor 1 and 2 directions based on joystick Y-axis (for forward/reverse)
    if (axisY < -100) {
        digitalWrite(IN1, HIGH);  // Motor 1 forward
        digitalWrite(IN3, HIGH);  // Motor 2 reverse
    } else if (axisY > 100) {
        digitalWrite(IN2, HIGH);  // Motor 1 reverse
        digitalWrite(IN4, HIGH);  // Motor 2 forward
    }

    // Handle motor 1 and 2 directions based on joystick X-axis (for turning)
    if (axisX < -100) {
        digitalWrite(IN1, HIGH);  // Motor 1 forward
        digitalWrite(IN4, HIGH);  // Motor 2 reverse
    } else if (axisX > 100) {
        digitalWrite(IN2, HIGH);  // Motor 1 reverse
        digitalWrite(IN3, HIGH);  // Motor 2 forward
    }
}

void processControllers() {
    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected() && ctl->hasData()) {
            if (ctl->isGamepad()) {
                processGamepad(ctl);
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());

    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);  // Optional: disables virtual controller

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void loop() {
    bool updated = BP32.update();

    if (updated) {
        processControllers();
    }

    delay(150);  // Small delay to avoid flooding
}