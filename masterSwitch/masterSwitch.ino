#include <FastLED.h>
#include <ArtnetWifi.h>
#include <Arduino.h>

// pin connected to the on/off state of the switch
const int switchOffPin= 12;     
const int switchOnPin = 13;     

// status led params
const int statusLedPin = 14;
const int numStatusLeds = 4;
CRGB statusLeds[numStatusLeds];

// strobo led params
const int stroboLedPin = 2;
const int numStroboLed = 21;
CRGB stroboLeds[numStroboLed];
     
// pin to send the sound signal
const int soundPin =  15;     

int buttonOffState = 1;         
int buttonOnState = 0;  
bool booting = false;

//Wifi settings
const char* ssid = "Fischnetz";
const char* password = "incubator";

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 6; 
// ip of the target 
const char host[] = "192.168.1.29";


// set up function
void setup() {
  // set-up serial for debug output
  Serial.begin(115200);
  delay(1000);
  Serial.print("Starting up...");
  ConnectWifi();

  // art net init
  artnet.begin(host);
  artnet.setLength(3); // check this param
  artnet.setUniverse(startUniverse);
  
  // initialize the sound pin as an output:
  pinMode(soundPin, OUTPUT);
  pinMode(soundPin, OUTPUT);
  // initialize the switch pin as an input:
  pinMode(switchOnPin, INPUT);
  pinMode(switchOffPin, INPUT);
  // set up the leds
  FastLED.addLeds<WS2812, statusLedPin, GRB>(statusLeds, numStatusLeds);
  FastLED.addLeds<WS2812, stroboLedPin, GRB>(stroboLeds, numStroboLed);


  // read the current state of the switch
  buttonOnState = digitalRead(switchOnPin);
  buttonOffState = digitalRead(switchOffPin);
  
  //switch sound off
  digitalWrite(soundPin, HIGH);
}


void loop() {
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
    // turn on strobo effect
    strobo();
  } else {
    // high means no sound
    digitalWrite(soundPin, HIGH);   
  }
  // switch is off
  if(buttonOffState == HIGH) {
    // turn on all leds in red
    setStatusLeds(CRGB::Red);
  }
  // switch is on
  if(buttonOnState == HIGH) {
    // turn on all leds in green
    setStatusLeds(CRGB::Green);
    Serial.println("Sending dmx signal");
    // TODO: set the right value at the right channel
    uint8_t i = 0;
    uint8_t j = 128;
    artnet.setByte(i, j);
    artnet.write();
   
  }
}

void strobo(){
  for(int i = 0; i < numStroboLed; i++) {
    stroboLeds[i] = CRGB::Blue;
    FastLED.show();   
  }
  delay(100);
  for(int i = 0; i < numStroboLed; i++) {
    stroboLeds[i] = CRGB::Black;
    FastLED.show();   
  }
  delay(100);
}

// set the status leds to a color
void setStatusLeds(CRGB color) {
  for(int i = 0; i < numStatusLeds; i++) {
    statusLeds[i] = color;
    FastLED.show();   
  }
}

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
