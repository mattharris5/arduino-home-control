/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 modified 03 Dec 2015
 by Matt Harris

 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(10, 0, 1, 101);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);
int LED = 7; 

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

#define BUFSIZ 100  //Buffer size for getting data
char clientline[BUFSIZ];  //string that will contain command data
int index = 0;  //clientline index
int pinCommand = 0;

void loop()
{
  index=0;  //reset the clientline index
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
//          if(digitalRead(LED) == HIGH) {
//            client.println("1");
//          } else {
//            client.println("0");
//          }
//          client.println(digitalRead(LED));
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        if(strstr(clientline,"/?L1=1")!=0) {  //look for the command to turn the led on
          pinCommand = 1;
        } else if(strstr(clientline,"/?L1=0")!=0) {  //look for command to turn led off. Note: If led is on, it will stay on until command is given to turn it off, even if multiple computers are on the site
          pinCommand = 2;
        } else {
          pinCommand = 0;
        }
      }
    }

    if(pinCommand == 1) {  
      digitalWrite(LED, HIGH);  //turn the led on
      Serial.println("Turned pin ON");
    } else if(pinCommand == 2) { 
      digitalWrite(LED, LOW);  //turn led off
      Serial.println("Turned pin OFF");
    }
    pinCommand = 0;
    client.println(digitalRead(LED));

    delay(1);
    client.stop();
    Serial.println("client disconnected");

  }
}
