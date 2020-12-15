#include <Arduino.h>

#define DEBUG true // switch to "false" for production

#define sensorTrigPin D5
#define sensorEchoPin D6
#define NB_TRYWIFI 20 // WiFi connection retries

long duration, distance; // Duration used to calculate distance

// **************
void loop();
void setup();
// **************

void setup()
{
    // only print debug messages to serial if we're in debug mode
    if (DEBUG == true) {
        Serial.print("Waking up ");
    }

    Serial.begin(9600);

    pinMode(sensorTrigPin, OUTPUT);
    pinMode(sensorEchoPin, INPUT);
}

void loop()
{
    digitalWrite(sensorTrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(sensorTrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensorTrigPin, LOW);

    duration = pulseIn(sensorEchoPin, HIGH);
    distance = duration / 58.2; // The echo time is converted into cm

    Serial.print("distance: ");
    Serial.println(distance);

    delay(500);
}
