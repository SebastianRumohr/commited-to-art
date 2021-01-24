#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <base64.h>
#include <math.h>
#include <ArduinoJson.h>
#include "booster.h"

#include "secrets.h"
// https://www.dfrobot.com/blog-854.html

#define BOOSTER_RESET_PIN 26
#define BOOSTER_CS_PIN 27
#define BOOSTER_LED_COUNT 16

#define BOOSTER_MAX_VALUE 72
#define BOOSTER_MAX_SATURATION 255

#define NODEMCU_SPI_MISO 12
#define NODEMCU_SPI_MOSI 13
#define NODEMCU_SPI_CLK 14
#define NODEMCU_SPI_SS 15

static const String requestUrl = "https://api.github.com/repos/" + String(GitHub.repository) + "/commits?per_page=" + String((int)BOOSTER_LED_COUNT, 10) + "&sha=" + String(GitHub.branch);
static const String requestAuthHeaderValue = "token " + String(GitHub.oAuthToken);

void startSequence()
{
  uint16_t hue = 0;
  for (uint8_t i = 0; i < BOOSTER_LED_COUNT; i++)
  {
    booster_sethsv(hue, BOOSTER_MAX_SATURATION, BOOSTER_MAX_VALUE);
    booster_setled(i);
    booster_show();
    hue += 22;
    delay(100);
  }
}

uint8_t getShas(DynamicJsonDocument doc, String *shas)
{
  if (!doc.is<JsonArray>())
  {
    return 0;
  }

  JsonArray array = doc.as<JsonArray>();
  uint8_t count = 0;
  for (JsonVariant value : array)
  {
    if (!value.is<JsonObject>())
    {
      continue;
    }

    JsonObject object = value.as<JsonObject>();
    shas[count] = String(object["sha"].as<char *>());
    count++;
  }

  return count;
}

uint8_t hexCharToNibble(char c)
{
  uint8_t byte = c;
  // transform hex character to the 4bit equivalent number, using the ascii table indexes
  if (byte >= '0' && byte <= '9')
  {
    byte = byte - '0';
  }
  else if (byte >= 'a' && byte <= 'f')
  {
    byte = byte - 'a' + 10;
  }
  else if (byte >= 'A' && byte <= 'F')
  {
    byte = byte - 'A' + 10;
  }

  return byte;
}

uint16_t generateColor(String sha)
{
  uint16_t color = 0;
  uint8_t length = sha.length();
  uint8_t value = 0;
  for (uint8_t i = 0; i < length; i += 2)
  {
    value ^= hexCharToNibble(sha[i]) << 4 | hexCharToNibble(sha[i + 1]);
  }

  color = round(value * (360.0 / 256.0));
  return color;
}

void generateColors(String *shas, uint16_t *colors, uint8_t length)
{
  for (uint8_t i = 0; i < length; i++)
  {
    colors[i] = generateColor(shas[i]);
    Serial.println(colors[i]);
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  WiFi.begin(WiFiConfig.ssid, WiFiConfig.password);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(250);
  }

  Serial.println("Connected to WiFi");
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Setting up booster..");
  SPI.begin(NODEMCU_SPI_CLK, NODEMCU_SPI_MISO, NODEMCU_SPI_MOSI, NODEMCU_SPI_SS);
  booster_setup(BOOSTER_LED_COUNT, BOOSTER_RESET_PIN, BOOSTER_CS_PIN);
  Serial.println("Booster ready");
  startSequence();
}

DynamicJsonDocument request()
{
  DynamicJsonDocument doc(2048);
  HTTPClient http;
  Serial.println(requestUrl);
  http.begin(requestUrl);
  http.addHeader("Authorization", requestAuthHeaderValue);

  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode == 200)
  {
    DynamicJsonDocument filter(200);
    filter[0]["sha"] = true;
    String payload = http.getString();
    deserializeJson(doc, payload, DeserializationOption::Filter(filter));
    serializeJsonPretty(doc, Serial);
    Serial.println("\n---");
  }
  else if (httpCode > 0)
  { //Check for the returning code
    String payload = http.getString();
    Serial.println(payload);
    deserializeJson(doc, "null");
  }
  else
  {
    Serial.println("Error on HTTP request");
    deserializeJson(doc, "null");
  }

  return doc;
}

void showError(uint16_t hue = 0)
{

  booster_setrgb(0, 0, 0);
  booster_setall();

  uint8_t errorLed = BOOSTER_LED_COUNT - 1;

  for (uint8_t i = 0; i < 5; i++)
  {
    booster_setrgb(0, 0, 0);
    booster_setled(errorLed);
    booster_show();
    delay(500);
    booster_sethsv(hue % 360, BOOSTER_MAX_SATURATION, BOOSTER_MAX_VALUE);
    booster_setled(errorLed);
    booster_show();
    delay(500);
  }
}

static String previousShas[BOOSTER_LED_COUNT];

uint8_t countNewShas(String *previous, String *current, uint8_t length)
{
  uint8_t index = 0;
  for (index = 0; index < length; index++)
  {
    if (current[index].equalsIgnoreCase(previous[0]))
    {
      break;
    }
  }

  return index;
}

void showResult(DynamicJsonDocument doc)
{
  String shas[BOOSTER_LED_COUNT];
  uint16_t colors[BOOSTER_LED_COUNT];
  uint8_t count = getShas(doc, shas);
  generateColors(shas, colors, count);

  uint8_t newCount = countNewShas(previousShas, shas, count);
  Serial.printf("%d new commits\n", newCount);

  for (uint8_t blink = 0; blink < 5; blink++)
  {
    if (blink > 0)
    {
      // off
      for (uint8_t i = 0; i < count; i++)
      {
        if (i < newCount)
        {
          booster_sethsv(0, 0, 0);
          booster_setled(BOOSTER_LED_COUNT - i - 1);
        }
      }
      booster_show();

      delay(500);
    }

    // on
    for (uint8_t i = 0; i < count; i++)
    {
      booster_sethsv(colors[i], BOOSTER_MAX_SATURATION, BOOSTER_MAX_VALUE);
      booster_setled(BOOSTER_LED_COUNT - i - 1);
    }

    booster_show();
    delay(500);
  }

  for (uint8_t i = 0; i < BOOSTER_LED_COUNT; i++)
  {
    previousShas[i] = shas[i];
  }
}

void loop()
{
  if ((WiFi.status() == WL_CONNECTED))
  {
    DynamicJsonDocument doc = request();

    if (doc.isNull())
    {
      showError();
    }
    else
    {
      showResult(doc);
    }
  }
  else
  {
    showError(240);
  }

  delay(10000);
}