#include <FastLED.h>
#include <ArtnetWifi.h>
#include <Arduino.h>

const int switchOffPin= 12;     
const int switchOnPin = 13;     


#define LED_PIN     14
#define NUM_LEDS    4
#define LED_TYPE    WS2812

#define LED_PIN_MAIN     2
#define NUM_LEDS_MAIN    20


CRGB leds[NUM_LEDS];
CRGB ledsMain[NUM_LEDS_MAIN];
     

// pin to send the sound signal
const int soundPin =  15;     

// variables will change:
// variable for reading the pushbutton status
int buttonOffState = 1;         
int buttonOnState = 0;  
bool booting = false;

//Wifi settings
const char* ssid = "Fischnetz";
const char* password = "incubator";

// Artnet settings
ArtnetWifi artnet;
// CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const int startUniverse = 6; 
const char host[] = "192.168.1.29"; // CHANGE FOR YOUR SETUP your destination

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(IPAddress(WiFi.localIP()));
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void setup() {
  // set-up serial for debug output
  Serial.begin(115200);
  delay(1000);
  Serial.print("Starting up...");
  ConnectWifi();

  // art net init
  artnet.begin(host);
  artnet.setLength(3);
  artnet.setUniverse(startUniverse);
  
  // initialize the LED pin as an output:
  pinMode(soundPin, OUTPUT);
  pinMode(soundPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(switchOnPin, INPUT);
  pinMode(switchOffPin, INPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, LED_PIN_MAIN, GRB>(ledsMain, NUM_LEDS_MAIN);

  
  buttonOnState = digitalRead(switchOnPin);
  buttonOffState = digitalRead(switchOffPin);
  //switch sound off
  
  digitalWrite(soundPin, HIGH);
}

void loop() {

  // turn on all leds in red
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
    FastLED.show();   
  }

  

  
  int preButtonOffState = buttonOffState; 
  
  // read the state of the pushbutton value:
  buttonOnState = digitalRead(switchOnPin);
  buttonOffState = digitalRead(switchOffPin);

  // switch was at off state an started moving to on state
  if (preButtonOffState == HIGH && buttonOffState == LOW) {
    booting = true;
  }
  // switch reached the on state
  if (buttonOnState  == HIGH || buttonOffState  == HIGH ) {
    booting = false;
  }
  

  if (booting) {
    digitalWrite(soundPin, LOW);
    for(int i = 0; i < NUM_LEDS_MAIN; i++) {
      ledsMain[i] = CRGB::Blue;
      FastLED.show();   
    }
    delay(100);
    for(int i = 0; i < NUM_LEDS_MAIN; i++) {
      ledsMain[i] = CRGB::Black;
      FastLED.show();   
    }
    delay(100);
  } else {
    // high means no sound
    digitalWrite(soundPin, HIGH);   
  }

  if(buttonOnState == HIGH) {
    Serial.println("Sending dmx signal");
    uint8_t dmxChannel = 5;
    uint8_t dmxValue = 128;
    // send dmx signal
    artnet.setByte(dmxChannel, dmxValue);
    // send out the Art-Net DMX data
    artnet.write();
  }
}
