#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClient.h>

#define ESP_WIFI_MODE 1 // WIFI_STA // WIFI_AP //WIFI_AP_STA
#define HOLD_TIME 2000

const char *ssid = "admin";
const char *password = "domestos1216";

const uint8_t btnGPIO = 13;

const uint8_t LED1GPIO = 0;  // red
const uint8_t LED2GPIO = 14; // green
const uint8_t LED3GPIO = 12; // white

const uint8_t defaulLED[] = {LED1GPIO, LED2GPIO, LED3GPIO};
const uint8_t algolLED[] = {LED3GPIO, LED2GPIO, LED1GPIO, LED2GPIO, LED3GPIO};

bool btnHold = false;
bool isPressBtn = false;
bool algoBlink = false;

uint32_t lastHoldTime = 0;
uint32_t currentDelay = 0;
uint32_t blinkTime = 1000;

bool siteBtnPressed = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial;
            text-align: center;
            margin: 0px auto;
            padding-top: 30px;
        }

        .button {
            padding: 10px 20px;
            font-size: 24px;
            text-align: center;
            outline: none;
            color: #fff;
            background-color: #2f4468;
            border: none;
            border-radius: 5px;
            box-shadow: 0 6px #999;
            cursor: pointer;
            -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
        }

        .button:hover {
            background-color: #1f2e45
        }

        .button:active {
            background-color: #1f2e45;
            box-shadow: 0 4px #666;
            transform: translateY(2px);
        }

        .leds {
            width: 100px;
            height: 100px;
            margin: 20px;
            display: inline-block;
            border: 5px solid black;
            border-radius: 50%;
        }

        .container {
            text-align: center;
        }

        #led1 {
            background-color: white;
        }

        @media screen and (max-width: 480px) {
            .leds {
                width: 50px;
                height: 50px;
            }

            .button {
                padding: 15px 100px 15px 10px;
                font-size: 10px;
            }

            h1 {
                font-size: 24px;
                padding-top: 20px;
            }
        }
    </style>
</head>

<body>
    <h1>ESP Pushbutton Web Server</h1>
    <!-- onmousedown / onmouseup - on PC/Laptop, ontouchend / ontouchstart - on mobile -->
    <button class="button" onmousedown="algorighm1('on_alg1');" ontouchstart="algorighm1('on_alg1');"
        onmouseup="algorighm1('off_alg1');" ontouchend="algorighm1('off_alg1');">Algorithm 1</button>


    <script>

        function algorighm1(x) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/" + x, true);
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/on_alg1", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    algoBlink = true;
    Serial.println("site bnt true");
    request->send(200, "text/plain", "ok"); });

  server.on("/off_alg1", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    algoBlink = false;
    Serial.println("site bnt false");

    request->send(200, "text/plain", "ok"); });


  server.onNotFound(notFound);
  server.begin();    

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



void setup()
{
  Serial.begin(115200);
  pinsSetup();

  initWiFi();
}

uint8_t currentLED = 0; 

void do_algorithm()
{
    if (millis() - currentDelay >= blinkTime)
    {
        currentDelay = millis();

        if (algoBlink)
        {
            if (currentLED > 0)
            {
                digitalWrite(algolLED[currentLED - 1], LOW);
            }
            else
            {
                digitalWrite(algolLED[sizeof(algolLED) / sizeof(algolLED[0]) - 1], LOW);
            }

            digitalWrite(algolLED[currentLED], HIGH);
            Serial.println(currentLED);
            Serial.println("algo blink");

            currentLED = (currentLED + 1) % (sizeof(algolLED) / sizeof(algolLED[0]));
        }
        else
        {
            if (currentLED > 0)
            {
                digitalWrite(defaulLED[currentLED - 1], LOW);
            }
            else
            {
                digitalWrite(defaulLED[sizeof(defaulLED) / sizeof(defaulLED[0]) - 1], LOW);
            }

            digitalWrite(defaulLED[currentLED], HIGH);
            Serial.println(currentLED);
            Serial.println("gefault blink");

            currentLED = (currentLED + 1) % (sizeof(defaulLED) / sizeof(defaulLED[0]));
        }
    }
}

void loop()
{
  buttonHold();
  do_algorithm();
}