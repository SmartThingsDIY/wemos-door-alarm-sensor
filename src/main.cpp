#include <Arduino.h>
#include "secrets.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DEBUG true    // switch to "false" before final installation
#define NB_TRYWIFI 20 // WiFi connection retries

#define sensorEchoPin D5
#define sensorTrigPin D6

WiFiClient espClient;
PubSubClient client(espClient);
long duration, distance; // Duration used to calculate distance

// **************
void loop();
void setup();
void disconnectWiFi();
long readSensor();
void connectToHass();
void connectToWiFi();
void publishAlarmToHass(long distance);
// **************

/**
 * This is a function used to get the reading
 * @return
 */
long readSensor()
{
    digitalWrite(sensorTrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(sensorTrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensorTrigPin, LOW);

    duration = pulseIn(sensorEchoPin, HIGH);

    return duration / 58.2; // The echo time is converted into cm
}

/**
 * Establishes WiFi connection
 * @return
 */
void connectToWiFi()
{
    int _try = 0;
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    if (DEBUG == true)
    {
        Serial.println("Connecting to Wi-Fi");
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        _try++;
        if (_try >= NB_TRYWIFI)
        {
            if (DEBUG == true)
            {
                Serial.println("Impossible to connect WiFi, going to deep sleep");
            }
        }
    }
    if (DEBUG == true)
    {
        Serial.println("Connected to Wi-Fi");
    }
}

/**
 * ensure that WiFi is shut down in an orderly fashion
 * This is done to save energy
 */
void disconnectWiFi()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();

    if (DEBUG == true)
    {
        Serial.println("Switching WiFi Off");
    }
}

/**
 * Establishes connection to Home Assistant MQTT Broker
 * @return
 */
void connectToHass()
{
    client.setServer(MQTT_SERVER, 1883);

    // Loop until we're reconnected
    while (!client.connected())
    {
        if (DEBUG == true)
        {
            Serial.print("Attempting MQTT connection...");
        }
        // Attempt to connect
        // If you do not want to use a username and password, change next line to
        // if (client.connect("ESP8266Client")) {
        if (client.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD))
        {
            if (DEBUG == true)
            {
                Serial.println("connected");
            }
        }
        else
        {
            if (DEBUG == true)
            {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
            }
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

/**
 * Publishes notification to MQTT topic
 * @return
 */
void publishAlarmToHass(long distance)
{
    bool publishState;

    // publish the reading to Hass through MQTT
    publishState = client.publish(MQTT_PUBLISH_TOPIC, String(distance).c_str(), true);
    client.loop();

    if (DEBUG == true)
    {
        Serial.println("Alarm sent to Hass.");
        Serial.print("state: ");
        Serial.println(publishState);
        Serial.print("data: ");
        Serial.println(String(distance).c_str());
    }
}

void setup()
{
    // only print debug messages to serial if we're in debug mode
    if (DEBUG == true)
    {
        Serial.print("Waking up ");
    }

    Serial.begin(9600);

    disconnectWiFi(); // no need to switch WiFi on unless we need it

    pinMode(sensorTrigPin, OUTPUT);
    pinMode(sensorEchoPin, INPUT);
}

void loop()
{
    distance = readSensor();

    if (distance < 15)
    {
        if (DEBUG == true)
        {
            Serial.print("Distance: ");
            Serial.print(distance);
            Serial.print("cm");
            Serial.println(" - Door is closed");
        }
    }
    else
    {
        if (DEBUG == true)
        {
            Serial.print("Distance:");
            Serial.print(distance);
            Serial.println(" - Door is open");
        }
        connectToWiFi();              // 1- connect to WiFi
        connectToHass();              // 2- connect to Home Assistant MQTT broker
        publishAlarmToHass(distance); // 3- publish the distance on the MQTT topic
        disconnectWiFi();             // 4- Disconnect WiFi to save energy
    }

    delay(4000); // 4 seconds
}
