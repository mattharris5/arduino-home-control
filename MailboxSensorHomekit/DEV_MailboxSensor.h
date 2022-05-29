/**
 * Creates a new Mailbox Sensor.
 */
struct DEV_MailboxSensor : Service::OccupancySensor {     // A standalone Temperature sensor

  SpanCharacteristic *occupancyDetected;
  SpanCharacteristic *cname;

  int sensorState = 0, lastState=0;         // variable for reading the sensor status
  int sensorPin, doorPin;
  
  DEV_MailboxSensor(int sensorPin, int doorPin) : Service::OccupancySensor(){       // constructor() method

    //  Possible Characteristics available in HomeKit:
    //  ----------------------------------------------
    //  OccupancyDetected (required)
    //  Name
    //  StatusActive
    //  StatusFault
    //  StatusTampered
    //  StatusLowBattery
      
    occupancyDetected = new Characteristic::OccupancyDetected(false);
    cname = new Characteristic::Name("Mail Delivered");
    this->sensorPin = sensorPin;
    this->doorPin = doorPin;

//    LOG1("Initializing mail sensor pin on pin %d and door sensor on %d", sensorPin, doorPin);
    
    // initialize the sensor pin as an input:
    pinMode(sensorPin, INPUT);     
    digitalWrite(sensorPin, HIGH); // turn on the pullup
    pinMode(doorPin, INPUT_PULLUP);

  } // end constructor

  void loop(){

    if (digitalRead(doorPin) == LOW) { // only do this if the door is closed

      // read the state of the sensor value:
      sensorState = digitalRead(sensorPin);
    
      // check if the sensor beam is broken and turn on the onboard LED
      if (sensorState == LOW) {
        digitalWrite(homeSpan.getStatusPin(), HIGH);
      } else {
        digitalWrite(homeSpan.getStatusPin(), LOW);
      }
  
      // If it's changed, update the Homekit value
      if (sensorState != lastState) {
//        LOG1("Mail sensor state is " + sensorState + "\n");
        if (sensorState == LOW) {
          occupancyDetected->setVal(true); // set the new status; this generates an Event Notification and also resets the elapsed time
          LOG1("You've got mail!\n");
        } else {
          occupancyDetected->setVal(false);
          LOG1("Mail picked up\n");
        }
      } 
      lastState = sensorState;
      
    } // door check
  } // loop
  
};
      
