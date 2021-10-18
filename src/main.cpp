/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <Arduino.h>
#include <BleKeyboard.h>
#include <FastLED.h>

BleKeyboard bleKeyboard("DMV-key");

const int NLEDS = 1;
CRGB leds[NLEDS];

const int onboardPIN = 39;
const int onboardLED = 27;

bool connected = false;

void setLeds(CRGB color) 
{
  for (int k=0; k<NLEDS; k++) leds[k] = color;
  FastLED.show();
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE!");
  bleKeyboard.begin();
  pinMode(onboardPIN, INPUT);
  FastLED.addLeds<WS2812, onboardLED, GRB>(leds, NLEDS);
  FastLED.setBrightness(2);
}

void loop() 
{DD  

  if(bleKeyboard.isConnected())
  {

    setLeds(CRGB::Green);

    if (!connected) {
      connected = true;
      Serial.println("Connected !");
    }

    if (digitalRead(onboardPIN) == LOW)
    {
      setLeds(CRGB::Yellow);
      Serial.println("BTN!");
      bleKeyboard.print("D");
      delay(300);

      // setLeds(CRGB::Yellow);
      // Serial.println("BUZZ!");
      // bleKeyboard.press(KEY_LEFT_CTRL);
      // delay(100);
      // bleKeyboard.releaseAll();
      // delay(200);
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


