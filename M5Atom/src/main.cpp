#include <Arduino.h>
#include <FastLED.h>
#include <ArduinoOSCWiFi.h>
#include <AsyncMqttClient.h>


// WiFi stuff
// const char* ssid = "dmv-wifi";          // Wifi SSID
// const char* pwd = "dmvdmvdmv";          // Wifi Password
const char* ssid = "interweb";          // Wifi SSID
const char* pwd = "superspeed37";          // Wifi Password

// MQTT stuff
IPAddress mqttbroker(0,0,0,0);          // Specify Mosquitto broker IP - will be update if "/broker" OSC message is received 

// OSC stuff
IPAddress oscserver(255,255,255,255);   // Will be update once connected to a /24 broadcast address
int oscPort_go = 3333;    // Default Max4Live OSC port
int oscPort_ping = 3334;  // AtomBridge OSC port
int oscPort_pong = 3335;  // this OSC listen

// Atom stuffs
const int NLEDS = 1;
CRGB leds[NLEDS];
const int onboardPIN = 39;
const int onboardLED = 27;

// Network
bool wifiConnected = false;
bool mqttConnected = false;
AsyncMqttClient mqttClient;
TimerHandle_t oscPingTimer;


// Set LED
void setLeds(CRGB color) 
{
  for (int k=0; k<NLEDS; k++) leds[k] = color;
  FastLED.show();
}

// CONNECT MQTT
void mqttConnect() {
  if (wifiConnected && !mqttConnected && mqttbroker[0] != 0) {
    mqttClient.setServer(mqttbroker, 1883);
    mqttClient.connect();
  }
}

// CONNECT WIFI
void wifiConnect() {
  if (!wifiConnected) {
    WiFi.begin(ssid, pwd);
  }
}

// OSC BEACON
void oscPing() {
  if (wifiConnected && !mqttConnected) {
    OscWiFi.send(oscserver.toString(), oscPort_ping, "/ping", WiFi.localIP().toString() );
    Serial.printf("OSC: /ping %s sent on %s:%d\n", WiFi.localIP().toString().c_str(), oscserver.toString().c_str(), oscPort_ping);
  }
}


// WIFI CALLABCK
void onWifiConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    wifiConnected = true;
    Serial.println("WIFI: Connected !");
    Serial.print("My IP is ");
    Serial.println(WiFi.localIP());

    // prepare OSC
    oscserver = WiFi.localIP();
    oscserver[3] = 255;
    Serial.print("OSC Broadcast to ");
    Serial.println(oscserver.toString());

    // prepare MQTT
    mqttConnect();
}

void onWifiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    wifiConnected = false;
    Serial.println("WIFI: lost connection");
}


// MQTT CALLBACK
void onMqttConnect(bool sessionPresent) {
  mqttConnected = true;
  Serial.println("MQTT: Connected.");
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  mqttConnected = false;
  Serial.println("MQTT: Disconnected.");
}



// SETUP
void setup() 
{
  Serial.begin(115200);
  Serial.println("AtomCtrl starting..");
  pinMode(onboardPIN, INPUT);
  FastLED.addLeds<WS2812, onboardLED, GRB>(leds, NLEDS);
  FastLED.setBrightness(2);

  WiFi.disconnect(true, true);  // disable wifi, erase ap info
  delay(300);
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(onWifiConnected, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(onWifiDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

  OscWiFi.subscribe(oscPort_pong, "/pong",
        [&](String& s) {
            Serial.print("/pong: setting broker at ");
            Serial.print(s); Serial.println();
            mqttbroker.fromString(s);

            oscserver = mqttbroker;
            oscPort_go = oscPort_ping;  // OSC change port, now that somebody answered on it !
            mqttConnect();
        }
    );

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  
  wifiConnect();

  oscPingTimer = xTimerCreate("pingTimer", pdMS_TO_TICKS(2000), pdTRUE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(oscPing));
  xTimerStart(oscPingTimer, 0);
}

// LOOP
void loop() 
{

  // WIFI OK
  if(wifiConnected)
  {
    // LEDS
    if (mqttConnected) setLeds(CRGB::Green);    // GREEN: MQTT ok
    else setLeds(CRGB::Yellow);                 // YELLOW: OSC fallback

    // OSC PONG
    OscWiFi.update();

    // BTN PUSHED
    if (digitalRead(onboardPIN) == LOW)
    {
      setLeds(CRGB::Blue);
    
      // Send with MQTT if available
      if (mqttConnected) mqttClient.publish("atom/go", 1, false, "0");

      // Fallback to OSC
      else OscWiFi.send(oscserver.toString(), oscPort_go, "/go");
      
      delay(300);
    }

  }

  // NO WIFI
  else
  {
    setLeds(CRGB::Red);
    delay(300);
  }

  
  delay(2);
}


