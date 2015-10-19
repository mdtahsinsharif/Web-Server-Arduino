/*  Muhammad Tahsin Sharif
 *  Lalit Lal
 *  Devansh Vaid
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <SPI.h>
#include <Ethernet.h>


/* Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground
   
*/
   
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/......................................................................../
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type
*/
/......................................................................../
  void displaySensorDetails(void)
  {
    sensor_t sensor;
    bmp.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);
  }
  
  
  //ETHERNET
  
  // Enter a MAC address and IP address for controller.
  // The IP address will be dependent on local network:
  byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x45 //assigning a unique number at the end
  };
  IPAddress ip(192, 168, 2, 3);
  
  // Initialize the Ethernet server library with an appropriate port for HTTP
  // (port 80 is default for HTTP):
  
    EthernetServer server(80);
    float temperature;
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    bmp.getTemperature(&temperature);
    sensors_event_t event;
    bmp.getEvent(&event);
  
  void setup(void) 
  {
    
    Serial.println("Pressure Sensor Test"); Serial.println("");
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  
    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    
    /* Initialise the sensor */
    if(!bmp.begin())
    {
      /* There was a problem detecting the BMP085 ... check your connections */
      Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
      while(1);
    }
    
    /* Display some basic information on this sensor */
    displaySensorDetails();
  }
  
  /......................................................................../
  /*
      Arduino loop function, called once 'setup' is complete (your own code
      should go here)
  */
  /......................................................................../
  
  void loop(void) 
  {
    EthernetClient client = server.available();
  
    if (client) {
      Serial.println("new client");
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 3");  // refresh the page automatically every 3 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            
            // output the value of each analog input pin
            
            for (int analogChannel = 4; analogChannel < 6; analogChannel++) {
              //int sensorReading = analogRead(analogChannel);
              //client.print("Temperature: ");
              //client.print(analogChannel);
              //client.print(" is ");
              //client.print(sensorReading);
              client.println("<br />");
  
              if (event.pressure)
              {
                /* Display atmospheric pressue in hPa */
                client.print("Pressure:    ");
                client.print(event.pressure);
                client.println(" hPa <br />");
  
                client.print("Temperature: ");
                client.print(temperature);
                client.println(" C <br/>");
  
                /* Then convert the atmospheric pressure, and SLP to altitude         */
                /* Update this next line with the current SLP for better results      */
                
                client.print("Altitude:    "); 
                client.print(bmp.pressureToAltitude(seaLevelPressure,
                                                    event.pressure)); 
                client.println(" m");
                client.println(" <br/>");
              }
              else
              {
                client.println("Sensor error");
              }
              delay(500);
              
            }
            client.println("</html>");
            break;
          }
          
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
    }
      // give the web browser time to receive the data
      delay(1);
      // close the connection:
      client.stop();
      client.println("client disconnected");
      Ethernet.maintain();
  }
  
  
 
