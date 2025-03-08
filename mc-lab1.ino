#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClient.h>

#define ESP_WIFI_MODE 1 // WIFI_STA // WIFI_AP //WIFI_AP_STA
#define HOLD_TIME 2000

const char *ssid = "admin";
const char *password = "domestos1216";

const uint8_t btnGPIO = 13;

const uint8_t LED1GPIO = 0;
const uint8_t LED2GPIO = 14;
const uint8_t LED3GPIO = 12;

const uint8_t defaulLED[] = {LED1GPIO, LED2GPIO, LED3GPIO};
const uint8_t algolLED[] = {LED3GPIO, LED2GPIO, LED1GPIO, LED2GPIO, LED3GPIO};

bool btnHold = false;
bool isPressBtn = false;
bool siteBtnHold = false;
bool algoBlink = false;

uint32_t lastHoldTime = 0;
uint32_t currentDelay = 0;
uint32_t blinkTime = 1000;

void pinsSetup()
{
  pinMode(btnGPIO, INPUT);
  for (uint8_t i = 0; i < 3; i++)
  {
    pinMode(defaulLED[i], OUTPUT);
  }
  for (uint8_t j = 0; j < 4; j++)
  {
    pinMode(algolLED[j], OUTPUT);
  }
}

uint8_t initWiFi()
{
  if (ESP_WIFI_MODE == 1)
  {
    WiFi.mode(WIFI_STA);
    // Connect to Wi-Fi network with SSID and password
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println();
    Serial.print("ESP IP Address: http://");
    Serial.println(WiFi.localIP());

    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
  }
  else if (ESP_WIFI_MODE == 2)
  {
    WiFi.mode(WIFI_AP);
    Serial.println("Setting AP (Access Point)…");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(ssid, NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
  }
  else
  {
    WiFi.mode(WIFI_OFF);
    Serial.println("Wifi of");
    return -1;
  }

  return 0;
}

void buttonHold()
{
  if (!digitalRead(btnGPIO))
  {
    if (!btnHold)
    {
      lastHoldTime = millis();
      btnHold = true;
      Serial.println("press");
    }

    if (millis() - lastHoldTime >= HOLD_TIME)
    {
      algoBlink = !algoBlink;
      Serial.println("Button is held");
    }
  }
  else
  {
    btnHold = false;
  }
}

// if (algoBlink)
// {
//   Serial.println("true");
// }

void setup()
{
  Serial.begin(115200);
  pinsSetup();

  // initLedsArray();
  // attachInterrupt(digitalPinToInterrupt(btnGPIO), ISRbtnChange, CHANGE);
  // timestamp = millis();
  initWiFi();
}

void do_algorithm()
{
  if (algoBlink)
  {
    for (int i = 0; i < sizeof(algolLED) / sizeof(algolLED[0]); i++)
    {
      if (millis() - currentDelay >= blinkTime)
      {
        currentDelay = millis();
        digitalWrite(algolLED[i], HIGH);

        if (i > 0)
        {
          digitalWrite(algolLED[i - 1], LOW);
        }
      }
    }
  }
  else
  {
    for (int e = 0; e < sizeof(defaulLED) / sizeof(defaulLED[0]); e++)
    {
      if (millis() - currentDelay >= blinkTime)
      {
        currentDelay = millis();
        digitalWrite(defaulLED[e], HIGH);

        if (e > 0)
        {
          digitalWrite(defaulLED[e - 1], LOW);
        }
      }
    }
  }
}

void loop()
{
  buttonHold();
  do_algorithm();
}