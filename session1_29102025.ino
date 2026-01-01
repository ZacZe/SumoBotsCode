#include <Bluepad32.h>

const int IN1 = 17; 
const int IN2 = 16; 
const int IN3 = 5;  
const int IN4 = 18; 

ControllerPtr myControllers[BP32_MAX_GAMEPADS];


// Called when a controller connects. Assings it to the first free controller slot
//
void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            Serial.printf("Controller connected at index %d\n", i);
            return;
        }
    }
    Serial.println("Controller connected, but no empty slot");
} // END onConnectedController


// Called when a controller disconnects; Clears its assigned controller slot
//
void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            Serial.printf("Controller disconnected from index %d\n", i);
            return;
        }
    }
} // END onDisconnectedController


// Does the Gamepad Processing
//
void processGamepad(ControllerPtr ctl) {
    int16_t axisX = ctl->axisX();
    int16_t axisY = ctl->axisRY();

    /* Braking (joysticks centered) */
    if (abs(axisX) < 50 && abs(axisY) < 50) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        return;
    }

    /* Stop motors first */
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    /* Forward / reverse */
    if (axisY < -100) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN3, HIGH);
    } else if (axisY > 100) {
        digitalWrite(IN2, HIGH);
        digitalWrite(IN4, HIGH);
    }

    /* Turning */
    if (axisX < -100) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN4, HIGH);
    } else if (axisX > 100) {
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
    }
} // END processGamepad


// IDK honestly
//
void processControllers() {
    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected() && ctl->hasData() && ctl->isGamepad()) {
            processGamepad(ctl);
        }
    }
} // END processControllers


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
} // END setup


void loop() {
    if (BP32.update()) {
        processControllers();
    }
} // END loop
