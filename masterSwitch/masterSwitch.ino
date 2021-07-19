/*
 * 
 * 
 * This project is the attic switch and controls the a light slow via a DMX signal
*/

#include <ArtnetWifi.h>
#include <FastLED.h>
#include <Arduino.h>

// ===================== IMPORTANT CONSTANTS COULD BE CAHNGED =====================
//Wifi settings
const char *ssid = "Fischnetz";     // CHANGE FOR YOUR SETUP
const char *password = "incubator"; // CHANGE FOR YOUR SETUP
const int startUniverse = 6;        // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const char host[] = "192.168.1.60"; // CHANGE FOR YOUR SETUP your destination

// ===================== IMPORTANT CONSTANTS COULD BE CAHNGED =====================

// after this line change only if you know what you are doing
// pin connected to the on/off state of the switch
const int switchOffPin = 12;
const int switchOnPin = 13;

// status led params
const int statusLedPin = 14;
const int numStatusLeds = 4;
CRGB statusLeds[numStatusLeds];

// strobo led params
const int stroboLedPin = 2; //
const int numStroboLed = 21;
CRGB stroboLeds[numStroboLed];

// pin to send the sound signal
const int soundPin = 15;

int buttonOffState = 1;
int buttonOnState = 0;
bool booting = false;

//Poti settings
// Analog pin
const int potiPins[] = {
    32,
    34,
    35};

uint8_t lastPotiValues[] = {
    0, 0, 0};

// the dead band for poti values
int MAX_DIFFERENCE_POTI_VALLUES = 5;
int NR_POTIS = 3;

// Artnet settings
ArtnetWifi artnet;

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean connection_state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (i > 20)
    {
      connection_state = false;
      break;
    }
    i++;
  }
  if (connection_state)
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return connection_state;
}

void setup()
{
  Serial.begin(115200);

  // connect to wifi
  ConnectWifi();

  // connect to artnet
  artnet.begin(host);
  artnet.setLength(3);
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

void loop()
{
  // for every poti forward the value as a dmx signal
  for (int i = 0; i < NR_POTIS; i++)
  {
    uint8_t normed_value = map(analogRead(potiPins[i]), 0, 4095, 0, 255);
    if (abs(lastPotiValues[i] - normed_value) > MAX_DIFFERENCE_POTI_VALLUES)
    {
      artnet.setByte(i, normed_value);
      lastPotiValues[i] = normed_value;
    }
  }

  // DB Schalter
  int preButtonOffState = buttonOffState;

  // read the state of the pushbutton value:
  buttonOnState = digitalRead(switchOnPin);
  buttonOffState = digitalRead(switchOffPin);

  // switch was at off state an started moving to on state
  if (preButtonOffState == HIGH && buttonOffState == LOW)
  {
    booting = true;
  }
  // switch reached the on state
  if (buttonOnState == HIGH || buttonOffState == HIGH)
  {
    booting = false;
  }

  if (booting)
  {
    digitalWrite(soundPin, LOW);
    // turn on strobo effect
    strobo();
  }
  else
  {
    // high means no sound
    digitalWrite(soundPin, HIGH);
  }
  // switch is off
  if (buttonOffState == HIGH)
  {
    // turn on all leds in red
    setStatusLeds(CRGB::Red);
    artnet.setByte(3, 0);
  }
  // switch is on
  if (buttonOnState == HIGH)
  {
    // turn on all leds in green
    setStatusLeds(CRGB::Green);
    artnet.setByte(3, 255);
  }

  artnet.write();
  delay(50);
}

void strobo()
{
  for (int i = 0; i < numStroboLed; i++)
  {
    stroboLeds[i] = CRGB::Blue;
    FastLED.show();
  }
  delay(100);
  for (int i = 0; i < numStroboLed; i++)
  {
    stroboLeds[i] = CRGB::Black;
    FastLED.show();
  }
  delay(100);
}

// set the status leds to a color
void setStatusLeds(CRGB color)
{
  for (int i = 0; i < numStatusLeds; i++)
  {
    statusLeds[i] = color;
    FastLED.show();
  }
}
