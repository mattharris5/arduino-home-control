#include <Encoder.h>

#define pinA_in 3
#define pinB_in 2
#define pinA_out 8
#define pinB_out 9
#define led 13
#define powerButton 5
#define powerTrigger 6
#define minVolume 0
#define maxVolume 40

Encoder volumeKnob(pinA_in, pinB_in);
bool currentPowerState = false;
int currentVolume = 0;
long knobPosition  = -999;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(pinA_out, OUTPUT);
  pinMode(pinB_out, OUTPUT);

  pinMode(led, OUTPUT);
  pinMode(powerButton, INPUT_PULLUP);
  pinMode(powerTrigger, OUTPUT);
  digitalWrite(powerTrigger, LOW);

  Serial.println("Ready");
}

void loop() {
  int sensorVal = digitalRead(powerButton);
  if (sensorVal == HIGH) {
    // not pressed
  } else {
    togglePower();
  }  

  long newPosition;
  newPosition = volumeKnob.read();
  if (newPosition != knobPosition && newPosition != 0) {
    Serial.print("Position = ");
    Serial.println(newPosition);
    if (newPosition % 4 == 0) {
      adjustVolume(newPosition);
      volumeKnob.write(0);
    }
    knobPosition = newPosition;
  }

}

void adjustVolume(int direction) {
//  Serial.print("direction = ");
//  Serial.println(direction);
  
  int leftPin = pinA_out;
  int rightPin = pinB_out;

  if(direction < 0) {
    leftPin = pinB_out;
    rightPin = pinA_out;
  }

  digitalWrite(leftPin, HIGH);
  delay(1);
  digitalWrite(rightPin, HIGH);
  delay(1);
  digitalWrite(leftPin, LOW);
  delay(1);
  digitalWrite(rightPin, LOW);
  delay(1);

  // Update the variable that tracks current volume
  if (direction < 0) {
    currentVolume--;
  } else {
    currentVolume++;
  }

  // Make sure we're not out of bounds
  if (currentVolume > maxVolume) {
    currentVolume = maxVolume;
  } else if (currentVolume < minVolume) {
    currentVolume = minVolume;
  }
  
  printStatus();
}

void togglePower() {
  Serial.println("Toggling power");
  digitalWrite(powerTrigger, HIGH);
  delay(500);
  currentPowerState = !currentPowerState;
  digitalWrite(powerTrigger, LOW);
  printStatus();
}

void printStatus() {
  Serial.print("Power: ");
  Serial.print(currentPowerState);
  Serial.print(", Volume: ");
  Serial.println(currentVolume);
  Serial.println();
}

