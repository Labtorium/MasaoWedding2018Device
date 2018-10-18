

/*  PulseSensor™ Starter Project and Signal Tester
    The Best Way to Get Started  With, or See the Raw Signal of, your PulseSensor™ & Arduino.

    Here is a link to the tutorial
    https://pulsesensor.com/pages/code-and-guide

    WATCH ME (Tutorial Video):
    https://www.youtube.com/watch?v=82T_zBZQkOE


  -------------------------------------------------------------
  1) This shows a live human Heartbeat Pulse.
  2) Live visualization in Arduino's Cool "Serial Plotter".
  3) Blink an LED on each Heartbeat.
  4) This is the direct Pulse Sensor's Signal.
  5) A great first-step in troubleshooting your circuit and connections.
  6) "Human-readable" code that is newbie friendly."

*/

#include <WiFi.h>
#include <WiFiUdp.h>

#define USE_ARDUINO_INTERRUPTS true


const char * SSID = "Buffalo-G-D120";
const char * PWD = "vbvs5ug45ewjd";
//const char * SSID = "ap-pc-55003a";
//const char * PWD = "12345678";
const char * REMOTE_UDP_IP = "192.168.11.15";
//const char * REMOTE_UDP_IP = "192.168.1.100";
const int REMOTE_UDP_PORT = 5001;
const int LOCAL_UDP_PORT = 5000;

const int SENSOR_ID = 2;
const int SENSOR_COUNT = 20;

int PulseSensorPurplePin = 36;
int raw_value = 0;
int thresh = 1610;
int bpm = 0;
int beat_count = 0;
unsigned long beat_time[SENSOR_COUNT];
bool risingFlag = false;
bool droppingFlag = false;

WiFiUDP udp;



void setup() {
  Serial.begin(9600);         // Set's up Serial Communication at certain speed.


  //Connect to the WiFi network
  WiFi.begin(SSID, PWD);
  Serial.println("");


  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //This initializes udp and transfer buffer
  udp.begin(LOCAL_UDP_PORT);
}




// The Main Loop Function
void loop() {

  raw_value = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.
  
  if (raw_value > thresh) {
    risingFlag = true;
    droppingFlag = false;
  } else {
    if (risingFlag == true) {
      if (beat_count <= SENSOR_COUNT) {
        beat_time[beat_count] = millis();
        beat_count++;
      } else {
        for (int i=0; i<SENSOR_COUNT; i++) {
          beat_time[i] = beat_time[i+1];
        }
        beat_time[SENSOR_COUNT] = millis();
        bpm = SENSOR_COUNT * 60 * 1000 / (beat_time[SENSOR_COUNT] - beat_time[0]);
      }
    }
    risingFlag = false;
    droppingFlag = true;
  }

  Serial.println(raw_value);                    // Send the Signal value to Serial Plotter.

  
  udp.beginPacket(REMOTE_UDP_IP, REMOTE_UDP_PORT);
  udp.printf("%d,%d,%d", SENSOR_ID, bpm, raw_value);
  udp.endPacket();

  delay(20);
}
