/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <Arduino.h>
#include <BleKeyboard.h>
#include <FastLED.h>

BleKeyboard bleKeyboard("Buzzer");

const int NLEDS = 25;
CRGB leds[NLEDS];

const int buzzerPIN = 32;
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
  pinMode(buzzerPIN, INPUT_PULLUP);
  pinMode(onboardPIN, INPUT);

  FastLED.addLeds<WS2812, onboardLED, GRB>(leds, NLEDS);
  FastLED.setBrightness(100);
}

void loop() 
{

  if(bleKeyboard.isConnected())
  {

    setLeds(CRGB::Green);

    if (!connected) {
      connected = true;
      Serial.println("Connected !");
    }

    if (digitalRead(buzzerPIN) == LOW) 
    {
      setLeds(CRGB::Yellow);
      Serial.println("BUZZ!");
      bleKeyboard.press(KEY_LEFT_CTRL);
      delay(100);
      bleKeyboard.releaseAll();
      delay(200);
    }

    if (digitalRead(onboardPIN) == LOW)
    {
      setLeds(CRGB::Yellow);
      Serial.println("BTN!");
      bleKeyboard.println("ok");
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


