int rForward = 16; 
int rBackward = 17; 
int lForward = 18; 
int lBackward = 5; // ass

int enableA = 22;
int enableB = 23; 

void setup() {
  // put your setup code here, to run once:
  pinMode(rForward, OUTPUT); 
  pinMode(rBackward, OUTPUT);
  pinMode(lForward, OUTPUT);
  pinMode(lBackward, OUTPUT); 

  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);

  digitalWrite(enableA, HIGH);
  digitalWrite(enableB, HIGH); 
}

void loop() {
  // put your main code here, to run repeatedly:
  moveForward();
  delay(1000);
  moveBackward();
  delay(1000);
  turnRight(90); 
  delay(1000);
  moveForward(); 
  stopAll();
  delay(1000);
  turnLeft(90);
  moveBackward();
}

void moveForward() {
  digitalWrite(rForward, HIGH);
  digitalWrite(rBackward, LOW);

  digitalWrite(lForward, HIGH);
  digitalWrite(lBackward, LOW);
}

void moveBackward() {
  digitalWrite(rForward, LOW);
  digitalWrite(rBackward, HIGH);

  digitalWrite(lForward, LOW);
  digitalWrite(lBackward, HIGH);
}

void turnRight(double degrees) {
  digitalWrite(rForward, LOW);
  digitalWrite(rBackward, HIGH);

  digitalWrite(lForward, HIGH);
  digitalWrite(lBackward, LOW);

  double timeDelay = map(degrees, 0, 180, 0, 2000);
  delay(timeDelay);

  digitalWrite(lForward, LOW);
  digitalWrite(rBackward, LOW);
}

void turnLeft(double degrees) {
  digitalWrite(rForward, HIGH);
  digitalWrite(rBackward, LOW);

  digitalWrite(lForward, LOW);
  digitalWrite(lBackward, HIGH);

  double timeDelay = map(degrees, 0, 180, 0, 2000);
  delay(timeDelay);

  digitalWrite(lBackward, LOW);
  digitalWrite(rForward, LOW);
}

void stopAll() {
  digitalWrite(rForward, LOW);
  digitalWrite(rBackward, LOW);

  digitalWrite(lForward, LOW);
  digitalWrite(lBackward, LOW);
}
