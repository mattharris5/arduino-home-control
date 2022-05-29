/**
 * Creates a new Mailbox Door Sensor.
 */
struct DEV_MailboxDoor : Service::ContactSensor { 

  SpanCharacteristic *doorOpen;
  SpanCharacteristic *cname;

  int sensorState = 0, lastState=0;         // variable for reading the sensor status
  int sensorPin;
  
  DEV_MailboxDoor(int sensorPin) : Service::ContactSensor(){       // constructor() method

    //  Possible Characteristics available in HomeKit:
    //  ----------------------------------------------
    //  ContactSensorState (required)
    //  Name
    //  StatusActive
    //  StatusFault
    //  StatusTampered
    //  StatusLowBattery
      
    doorOpen = new Characteristic::ContactSensorState(true);
    cname = new Characteristic::Name("Mailbox Door Open");

    this->sensorPin = sensorPin;
    
    // initialize the sensor pin as an input:
//    LOG1("Initializing door sensor pin on pin %d", sensorPin);
    pinMode(sensorPin, INPUT_PULLUP);
//    digitalWrite(sensorPin, HIGH); // turn on the pullup

  } // end constructor

  void loop(){

    // read the state of the sensor value:
    sensorState = digitalRead(sensorPin);
  
    // If it's changed, update the Homekit value
    if (sensorState != lastState) {
//      LOG1("Door sensor state is " + sensorState + "\n");
      if (sensorState == LOW) {
        doorOpen->setVal(true); // set the new status; this generates an Event Notification and also resets the elapsed time
        LOG1("Door is closed\n");
      } else {
        doorOpen->setVal(false);
        LOG1("Door is open - someone's fetching the mail\n");
      }
    } 
    lastState = sensorState;
    
  } // loop
  
};
      
