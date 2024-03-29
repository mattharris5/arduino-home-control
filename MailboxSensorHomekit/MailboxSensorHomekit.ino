/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2020 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/
 

#include "HomeSpan.h" 
#include "DEV_MailboxSensor.h"
#include "DEV_Identify.h"
#include "DEV_MailboxDoor.h"

#define SENSORPIN 16
#define DOORPIN 21
#define ONBOARD_LED 2

void setup() {
  Serial.begin(115200);

  // Enable web logging -- disabled because v1.5.1 seems to break this board
  // homeSpan.enableWebLog(50,"pool.ntp.org","PST","log"); 

  // Setup the Homespan bridge and sensors
  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");
  homeSpan.setStatusPin(ONBOARD_LED);

  new SpanAccessory();  
    new DEV_Identify("Bridge #1","HomeSpan","123-ABC","HS Bridge","0.9",3);
    new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      
  new SpanAccessory();                                                          
    new DEV_Identify("Mailbox Sensor","HomeSpan","MKH01","HiLetgo ESP-WROOM-32 ESP-32S","0.9",0);
    new DEV_MailboxSensor(16, 21); // defined in DEV_MailboxSensor.h
  
  new SpanAccessory();                                                          
    new DEV_Identify("Mailbox Door","HomeSpan","MKH01","HiLetgo ESP-WROOM-32 ESP-32S","0.9",0);
    new DEV_MailboxDoor(21); // defined in DEV_MailboxDoor.h

} // end of setup()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // end of loop()
