/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <Arduino.h>
#include <FastLED.h>
#include <ArduinoOSCWiFi.h>

const int NLEDS = 1;
CRGB leds[NLEDS];

const int onboardPIN = 39;
const int onboardLED = 27;

bool connected = false;

// WiFi stuff
const char* ssid = "dmv-wifi";
const char* pwd = "dmvdmvdmv";

IPAddress broadcast(255,255,255,255);
const int oscPort_out = 3333;


void setLeds(CRGB color) 
{
  for (int k=0; k<NLEDS; k++) leds[k] = color;
  FastLED.show();
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting!");
  pinMode(onboardPIN, INPUT);
  FastLED.addLeds<WS2812, onboardLED, GRB>(leds, NLEDS);
  FastLED.setBrightness(2);

  WiFi.disconnect(true, true);  // disable wifi, erase ap info
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);


}

void loop() 
{

  if(WiFi.status() == WL_CONNECTED)
  {

    if (!connected) {
      connected = true;
      Serial.println("Connected !");
      Serial.print("My IP is ");
      Serial.println(WiFi.localIP());

      broadcast = WiFi.localIP();
      broadcast[3] = 255;
      Serial.print("Broadcast to ");
      Serial.println(broadcast.toString());
    }
    setLeds(CRGB::Green);

    if (digitalRead(onboardPIN) == LOW)
    {
      setLeds(CRGB::Yellow);
      Serial.println("BTN!");
      OscWiFi.send(broadcast.toString(), 3333, "/go");
      delay(300);
    }

  }
  else
  {
    Serial.println("Not connected.. ");
    connected = false;
    setLeds(CRGB::Red);
    delay(1000);
  }

  
  delay(2);
}


