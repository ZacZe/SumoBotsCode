#include <Bluepad32.h>

/* Motor pins */
const int IN1 = 17; 
const int IN2 = 16; 
const int IN3 = 5;  
const int IN4 = 18;

/* Buzzer pin */
const int BUZZER_PIN = 19;

/* Controller storage */
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

/* ---------------- NOTES (Hz) ---------------- */
#define A3 220
#define B3 247
#define C4s 277
#define D4 294
#define E4 330
#define F4s 370

#define A4 440
#define B4 494
#define C5s 554
#define D5 587
#define E5 659
#define F5s 740

/* ------------- MELODY ---------------- */
/* Never Gonna Give You Up (excerpt you gave) */

int melody[] = {
  B3, C5s, D5, D5, E5, C5s, B3, A3,
  B3, B3, C5s, D5, B3, A3, A4, A4, E5,
  B3, B3, C5s, D5, B3, D5, E5, C5s, B3
};

/* Durations (ms) – tweak freely */
int durations[] = {
  250,250,250,250,250,250,250,500,
  250,250,250,250,250,250,250,250,500,
  250,250,250,250,250,250,250,500
};

const int MELODY_LENGTH = sizeof(melody) / sizeof(int);

/* Playback state */
bool playing = false;
int noteIndex = 0;
unsigned long noteStart = 0;

/* ------------------------------------------------ */

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

/* ---------------- BUZZER PLAYER ---------------- */
void updateMelody() {
  if (!playing) return;

  unsigned long now = millis();

  if (noteIndex >= MELODY_LENGTH) {
    noTone(BUZZER_PIN);
    playing = false;
    return;
  }

  if (now - noteStart >= durations[noteIndex]) {
    noteIndex++;
    noteStart = now;

    if (noteIndex < MELODY_LENGTH) {
      tone(BUZZER_PIN, melody[noteIndex]);
    }
  }
}

/* ---------------- GAMEPAD ---------------- */
void processGamepad(ControllerPtr ctl) {

  /* R1 starts the song */
  if ((ctl->buttons() & BUTTON_R1) && !playing) {
    playing = true;
    noteIndex = 0;
    noteStart = millis();
    tone(BUZZER_PIN, melody[0]);
  }

  int16_t axisX = ctl->axisX();
  int16_t axisY = ctl->axisRY();

  if (abs(axisX) < 50 && abs(axisY) < 50) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    return;
  }

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  if (axisY < -100) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN3, HIGH);
  } else if (axisY > 100) {
    digitalWrite(IN2, HIGH);
    digitalWrite(IN4, HIGH);
  }

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
    if (ctl && ctl->isConnected() && ctl->hasData() && ctl->isGamepad()) {
      processGamepad(ctl);
    }
  }
}

/* ---------------- SETUP ---------------- */
void setup() {
  Serial.begin(115200);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

/* ---------------- LOOP ---------------- */
void loop() {
  if (BP32.update()) {
    processControllers();
  }

  updateMelody();
}
