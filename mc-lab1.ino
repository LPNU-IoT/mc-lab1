#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define ESP_WIFI_MODE 1 // WIFI_STA // WIFI_AP //WIFI_AP_STA
#define HOLD_TIME 500

const char *ssid = "admin";
const char *password = "domestos1216";

const uint8_t btnGPIO = 13;

const uint8_t LED1GPIO = 0;
const uint8_t LED2GPIO = 14;
const uint8_t LED3GPIO = 12;

const uint8_t defaulLED = [ LED1GPIO, LED2GPIO, LED3GPIO ];
const uint8_t algolLED = [ LED3GPIO, LED2GPIO, LED1GPIO, LED2GPIO, LED3GPIO ];

bool btnHold = false;
bool algoLED = false;
bool isPressBtn = false;
bool siteBtnHold = false;

uint32_t lastHoldTime = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        padding-top: 50px;
      }
      .button {
        padding: 15px 30px;
        font-size: 20px;
        color: white;
        background-color: #007bff;
        border: none;
        border-radius: 5px;
        cursor: pointer;
      }
      .button:hover {
        background-color: #0056b3;
      }
    </style>
  </head>
  <body>
    <h1>ESP Web Interface</h1>
    <button class="button" onclick="sendRequest()">Hold me</button>

    <script>
      function sendRequest() {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/button_press", true);
        xhr.send();
      }
    </script>
  </body>
</html>

)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void pinsSetup()
{
    pinMode(btnGPIO, INPUT);
    for (uint8_t i = 0; i < 3; i++)
    {
        pinMode(defaulLED[i], OUTPUT);
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

    // Send web page to client
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });

    server.on("/on_alg1", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    siteBtnPressed = true;
    request->send(200, "text/plain", "ok"); });

    server.on("/off_alg1", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    siteBtnPressed = false;
    request->send(200, "text/plain", "ok"); });

    server.on("/status_led_1", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", String(digitalRead(LED1.GPIO_PIN)).c_str()); });

    server.on("/status_led_2", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", String(digitalRead(LED2.GPIO_PIN)).c_str()); });

    server.on("/status_led_3", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", String(digitalRead(LED3.GPIO_PIN)).c_str()); });

    server.onNotFound(notFound);
    server.begin();

    return 0;
}

void buttonHold()
{
    isPressBtn = digitalRead(btnGPIO);
    if (isPressBtn)
    {
        if (millis() - lastHoldTime >= HOLD_TIME)
        {

            lastHoldTime = millis();
            btnHold = true;
            algoLED = !algoLED;
        }
    }
}

void chechSiteButton()
{
    if (siteBtnPressed)
    {
        btnPressed = true;
        msgAboutButtonSended = true;
    }
    else if (!siteBtnPressed && msgAboutButtonSended)
    {
        btnPressed = false;
        msgAboutButtonSended = false;
    }
}

void setup()
{
    Serial.begin(115200);
    pinsSetup();

    initLedsArray();
    attachInterrupt(digitalPinToInterrupt(btnGPIO), ISRbtnChange, CHANGE);
    timestamp = millis();
    initWiFi();
}

void do_algorithm()
{
    if (millis() - timestamp >= DELAY_BETWEEN_BUTTONS)
    {
        timestamp = millis();

        digitalWrite(currentLedPointer->prevLed->GPIO_PIN, LOW);
        digitalWrite(currentLedPointer->GPIO_PIN, HIGH);
        digitalWrite(currentLedPointer->prevLed->GPIO_PIN, LOW);

        currentLedPointer = direction ? currentLedPointer->nextLed : currentLedPointer->prevLed;
    }
}

void loop()
{
    chechSiteButton();
    btnChange();
    do_algorithm();
}