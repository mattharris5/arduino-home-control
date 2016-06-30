#include <Encoder.h>
#include <SPI.h>
#include <Ethernet.h>

#define pinA_in 3
#define pinB_in 2
#define pinA_out 8
#define pinB_out 9
#define led 13
#define powerButton 5
#define powerTrigger 6
#define minVolume 0
#define maxVolume 40
#define maxRequestVolume 30

// Rotary encoder config
Encoder volumeKnob(pinA_in, pinB_in);
bool currentPowerState = false;
int currentVolume = 0;
long knobPosition  = -999;
unsigned long lastVolumeResetTime;
unsigned long volumeResetInterval = 5 * 60 * 1000; // reset volume every 5 minutes

// Ethernet server config
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
IPAddress ip(10, 0, 1, 102);
EthernetServer server(80);
#define BUFSIZ 100  //Buffer size for getting data
char clientline[BUFSIZ];  //string that will contain command data
int index = 0;  //clientline index
String httpCommand = "";
int requestedVolume = 0;

//
// SETUP
//
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

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  resetVolume(); // reset to zero

  Serial.println("Ready");
}

//
// LOOP
//
void loop() {
  hijackPowerButton();
  hijackVolumeKnob();
  respondToEthernet();
}

//
// UTILITY FUNCTIONS
//
void adjustVolume(int direction) { 
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
  } else if (currentVolume < minVolume) { // - 20) { // allow less than zero to allow manual re-sync
    currentVolume = minVolume;
  }
  
//  printStatus();
}

void togglePower() {
  Serial.println("Toggling power");
  digitalWrite(powerTrigger, HIGH);
  delay(500);
  currentPowerState = !currentPowerState;
  digitalWrite(powerTrigger, LOW);
  printStatus();
}

String currentStatus() {
  String ret = "Power: ";
  ret += currentPowerState;
  ret += ", Volume: ";
  ret += currentVolume;
  return ret;
}

void printStatus() {
  Serial.println(currentStatus());
  Serial.println();
}

void hijackPowerButton() {
    int sensorVal = digitalRead(powerButton);
  if (sensorVal == HIGH) {
    // not pressed
  } else {
    togglePower();
  }
}

void hijackVolumeKnob() {
  long newPosition;
  newPosition = volumeKnob.read();
  if (newPosition != knobPosition && newPosition != 0) {
    if (newPosition % 4 == 0) {
      adjustVolume(newPosition);
      volumeKnob.write(0);
    }
    knobPosition = newPosition;
  }
}

bool setPower(bool newStatus) {
  if ((newStatus && !currentPowerState) || (!newStatus && currentPowerState)) {
     togglePower();
  }
  return currentPowerState;
}

int currentPowerStateInt() {
  int pwrInt = (currentPowerState ? 1 : 0);
  return pwrInt;
}

int setVolume(int newVolume) {
  if (millis() > lastVolumeResetTime + volumeResetInterval) {
    resetVolume();
  }
  if (newVolume > maxVolume || newVolume < minVolume) {
    return currentVolume;
  }
  while(newVolume != currentVolume) {
    adjustVolume(newVolume - currentVolume);
    delay(5);
  }
  return currentVolume;
}

int setVolumePercent(int newVolumePercent) {
  int newVolume = (int) newVolumePercent * (maxRequestVolume / 100.0);
  setVolume(newVolume);
  Serial.print("Request was for ");
  Serial.print(newVolumePercent);
  Serial.print("%, setting actual volume to ");
  Serial.println(newVolume);
  return newVolumePercent;
}

int currentVolumePercent() {
  return (int) currentVolume * (100.0 / maxRequestVolume);
}

void resetVolume() {
  Serial.println("Resetting volume");
  for(int i = 40; i > 0; i--) {
    adjustVolume(-1);
    delay(1);
  }
  lastVolumeResetTime = millis();
}

void respondToEthernet() {
  index = 0;  //reset the clientline index
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if(index<BUFSIZ)  //Only add data if the buffer isn't full.
        {
          clientline[index]=c;
          index++;
        }
        if (c == '\n' && currentLineIsBlank)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println();
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        if(strstr(clientline,"/?power=on")!=0) {  //look for the command to turn the device on
          httpCommand = "powerOn";
        } else if(strstr(clientline,"/?power=off")!=0) {  //look for command to turn device off
          httpCommand = "powerOff";
        } else if(strstr(clientline,"/?volume=status")!=0) { // check current volume
          httpCommand = "checkVolume";
        } else if(strstr(clientline,"/?volume=")!=0) { // set the volume
          httpCommand = "setVolume";
          String clientlineStr = String(clientline);
          int start = clientlineStr.indexOf("/?volume=");
          String requestedVolumeStr = clientlineStr.substring(start+9, start+12);
          requestedVolume = requestedVolumeStr.toInt();
        } else {
          httpCommand = "";
        }
      }
    }

    // handle commands
    if(httpCommand == "powerOn") { 
      setPower(true);
      client.println(currentPowerStateInt());
    } else if(httpCommand == "powerOff") { // turn off
      setPower(false);
      client.println(currentPowerStateInt());
    } else if(httpCommand == "setVolume") {
      int newVolume = setVolumePercent(requestedVolume);
      client.println(newVolume);
    } else if(httpCommand == "checkVolume") {
      client.println(currentVolumePercent());
    } else {
      client.println(currentPowerStateInt());
    }
    httpCommand = "";
    
    delay(1);
    client.stop();
    Serial.println("client disconnected");
  }
}

