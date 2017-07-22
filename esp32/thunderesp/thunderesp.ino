// WiFi includes
#include <WiFi.h>

// OTA Includes
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);

#define FAN 16
#define LEDS 25

#include <NeoPixelBus.h>

const uint16_t PixelCount = 4;
const uint8_t PixelPin = LEDS;
#define colorSaturation 128

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);

uint32_t interval = 1000;
uint32_t lastMillis = 0;
bool LEDS_ON = false;

void setup()
{
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  
  Serial.begin(115200);
  // Align text vertical/horizontal center
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, "Starting WiFi Manager.");
  display.display();
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();

  //set custom ip for portal
  //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, HIGH);

  ArduinoOTA.begin();
  ArduinoOTA.onStart([]() {
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - 10, "OTA Update");
    display.display();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
  });

  ArduinoOTA.onEnd([]() {
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, "Restart");
    display.display();
  });

  // Align text vertical/horizontal center
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_10);
  display.drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, "Ready for OTA:\n" + WiFi.localIP().toString());
  display.display();

  strip.Begin();
  strip.Show();

  lastMillis = millis();
}

void loop()
{
  ArduinoOTA.handle();
  
  if(millis()-lastMillis > interval)
  {
    if(LEDS_ON)
    {
      // turn off the pixels
      strip.SetPixelColor(0, black);
      strip.SetPixelColor(1, black);
      strip.SetPixelColor(2, black);
      strip.SetPixelColor(3, black);
    }
    else
    {
      strip.SetPixelColor(0, red);
      strip.SetPixelColor(1, green);
      strip.SetPixelColor(2, blue);
      strip.SetPixelColor(3, white);
    }
    strip.Show();
    LEDS_ON = !LEDS_ON;
    lastMillis = millis();
  }

}


