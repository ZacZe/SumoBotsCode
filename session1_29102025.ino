/*
int rForward = 16; 
int rBackward = 17; 
int lForward = 18; 
int lBackward = 5; // ass 
*/

#include <Bluepad32.h>
/*
#define IN1 16 // Motor 1 forward
#define IN2 17  // Motor 1 reverse
#define IN3 18  // Motor 2 reverse
#define IN4 5  // Motor 2 forward
*/

#define IN1 17 // Motor 1 forward
#define IN2 16  // Motor 1 reverse
#define IN3 5  // Motor 2 reverse
#define IN4 18  // Motor 2 forward
#define START 19

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

bool started = false;       // keeps track of motor driver state
bool lastXState = false;    // remembers last X button state

void processGamepad(ControllerPtr ctl) {
    int16_t axisX = ctl->axisX();
    int16_t axisY = ctl->axisRY();

    // Get current X button state
    bool currentX = ctl->buttonPressed(0);

    // Toggle on rising edge
    if (currentX && !lastXState) {
        started = !started;          // flip state
        digitalWrite(START, started ? HIGH : LOW);
    }

    // Remember the current X state for next loop
    lastXState = currentX;

    // Braking (joystick centered)
    if (abs(axisX) < 50 && abs(axisY) < 50) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        return;
    }

    // Stop motors first
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    // Forward/reverse
    if (axisY < -100) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN3, HIGH);
    } else if (axisY > 100) {
        digitalWrite(IN2, HIGH);
        digitalWrite(IN4, HIGH);
    }

    // Turning
    if (axisX < -100) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN4, HIGH);
    } else if (axisX > 100) {
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
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
    pinMode(START, OUTPUT);
    digitalWrite(START, LOW);
}

void loop() {
    bool updated = BP32.update();

    if (updated) {
        processControllers();
    }

    delay(100);  // Small delay to avoid flooding
} 
