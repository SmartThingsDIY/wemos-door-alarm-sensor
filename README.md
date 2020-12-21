# DIY Door Alarm Sensor With Home Assistant...
Or any MQTT based system

About
=====
This article accompanies the "DIY Door Alarm Sensor" YouTube series. It contains the code, libraries, diagrams, 3D print files and more information that I promised in the videos: [**DIY Door Alarm Sensor**](https://youtu.be/6FVJRkGDwwI)

BUY VS BUILD
========
You can always go ahead and buy a ready-to-use a solution like this [Ring Alarm Contact Sensor ](https://www.amazon.ca/Ring-Alarm-Contact-Sensor/dp/B07ZB2QLC2) but these devices usually would only work with their respective apps, and some of them would require you to pay extra in order to receive push notifications, or enable cloud usage. Now, this article and its video provide a step-by-step guide and links to everything you need to build a similar device that accomplishes the same thing.

<img align="right" src="https://images-na.ssl-images-amazon.com/images/I/51edNlMRAjL._SL1000_.jpg" style="max-width:100%;" height="400">

In addition to detecting when a door or window are open and sending notifications. This device:
* Goes to deep sleep and only wakes up every 30sec (configurable) to save energy
* When motion is detected, only then, the connection to WiFi is made, again, to save energy
* Has a removable LiPo battery for easy of charge
* Clever wiring of the motion sensor to extend its life expectancy (more details in the wiring section)
* Sends notifications (and any type of data) to an MQTT topic, enlarging the possibilities of what you can do with the data

In my case, I will be sending the notifications to my [Home Assistant](https://www.home-assistant.io/) setup in order to trigger a pre-defined automation that:
* Pushes actionable notifications to our phone devices through [Home Assistant companion apps](https://companion.home-assistant.io/)
* Converts notification alarm to a text (Alexa TTS capabilities) and play it through the Amazon Echo devices around the house
* And just in case we managed to miss these, it turns the kitchen LED light to RED informing us of an emergency
* Repeats every 5min until someone explicitly acknowledges the issue.

The beauty of this is I can decide to trigger any type of notifications in the future such as playing an alarm sound or sending SMS notifications...

So without further ado, let's start with a list of all hardware components, Apps and Libraries I've used in this project:

⚡️ COMPONENTS AND SUPPLIES
=====

<img align="right" src="https://camo.githubusercontent.com/57ff718ea8c8c9b2e0509fe98f4a37096c85b5cc9d0da1f301255006e75b3385/68747470733a2f2f696d616765732d6e612e73736c2d696d616765732d616d617a6f6e2e636f6d2f696d616765732f492f343138427069687a4f384c2e6a7067" style="max-width:100%;" height="400">

*   [WEMOS D1 R2 (ESP8266 ESP-12F)](https://www.banggood.com/Geekcreit-D1-mini-V2_2_0-WIFI-Internet-Development-Board-Based-ESP8266-4MB-FLASH-ESP-12S-Chip-p-1143874.html?cur_warehouse=CN&rmmds=search/)
*   [HC-SR04 Ultrasonic Sensor](https://www.banggood.com/Wholesale-Geekcreit-Ultrasonic-Module-HC-SR04-Distance-Measuring-Ranging-Transducers-Sensor-DC-5V-2-450cm-p-40313.html?cur_warehouse=CN&rmmds=search/)
*   [5 LiPo Batteries And Charger](https://www.amazon.ca/gp/product/B0795F139D)
*   [Spade 2P Cable Lead Plug](https://www.amazon.ca/gp/product/B07YQY9V6F/)
*   [Solder Kit](https://www.amazon.ca/-/fr/gp/product/B01N46T138/)
*   [Helping Hands for soldering](https://www.amazon.ca/gp/product/B002PIA6Z4)
*   [Screwdriver Set](https://www.amazon.ca/Precision-Screwdriver-Lifegoo-Eyeglasses-Electronic/dp/B07XCWT3W8/)
*   [Breadboard](https://amzn.to/2Ei40tP) - [Jumper Wires](https://amzn.to/2Ehh2ru) - [Male to Male Jumper Wires + Tweezer](https://amzn.to/3jcf9eX)

🖥 APPS
=====

*   [VSCode](https://code.visualstudio.com/)
*   [AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-install.html)
*   [Fritzing](https://fritzing.org/)
*   [PlatformIO](https://platformio.org/)

📦 Libraries
=====
*   [ESP8266WiFi](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html)
*   [PubSubClient](https://www.arduinolibraries.info/libraries/pub-sub-client)

Hardware Overview: HC-SR04 Ultrasonic Sensor
===============
<img align="right" src="https://imgaz1.staticbg.com/thumb/large/oaupload/banggood/images/C8/38/4d9de07a-5bb8-4522-85d6-bdbe7011ae1e.jpg" style="max-width:100%;" height="350">

The Ultrasonic Module HC-SR04 can measure the distance to an object by using echo. It can measure the distance between the sensor and the object by the time from the sound wave of a specific frequency to the time when the sound wave is heard to bounce back

This small module is perfect for all kind of projects that need distance measurements, e.g. avoiding obstacles. It provides 2cm-500cm non-contact measurement function with a ranging hight accuracy that can reach to 3mm

I will be placing this module on my door where the distance to the wall is fixed while the door is closed, but when opened, the distance between the door and the wall obviously increases. This increase in distance will be detected by the HC-SR04 module which will trigger the alarm


<img align="left" src="https://m.media-amazon.com/images/S/aplus-media/sc/5ce209b0-d545-4471-b5e2-4b8ecf071ceb.__CR0,0,300,300_PT0_SX300_V1___.jpg" style="max-width:100%;" height="250">

**HC-SR04 working principle:**
The module automatically sends eight 40KHz square wave to detect whether a signal is returned, if an object is detected, Echo pin will be high level, and based on the different distance, it will take the different duration of high level
So we can calculated the distance easily:

* The distance = ((Duration of high level) * (Sonic :340m/s)) / 2
* **Distance L = 1/2 × T × C**


Hardware Overview: WEMOS R1 D2
===============
<img align="right" src="https://images-na.ssl-images-amazon.com/images/I/61sU-gLZncL._AC_SL1001_.jpg" style="max-width:100%;" height="300">

I've decided to use the [WEMOS R1 D2](https://www.amazon.com/Sensor-Module-Detection-Surface-Arduino%EF%BC%8810pcs%EF%BC%89/dp/B07THDH7Y4/ref=sr_1_7?dchild=1&keywords=6+Pack+Water+Level+Sensor%2C+Droplet+Depth+Detection+Sensor+for+Arduino&sr=8-7) microchip for this project because it has a small size that saves space and can easily fit in a printable 3D case, but also because of a few reasons:
* It has an Analog pin
* It has WiFi capabilities
* It supports OTA online: Updating code Over-The-Air
* 4MB of memory which more than enough for our needs
* And because it's compatible with Arduino IDE, making the programming easy

Hardware Overview: LiPo Battery
===============
<img align="right" src="https://images-na.ssl-images-amazon.com/images/I/31jNdqR1-yL._AC_.jpg" style="max-width:100%;" height="150">

A Lithium Polymer battery, or more correctly Lithium-ion Polymer battery (abbreviated as **LiPo**, **LIP**, **Li-poly**, **lithium-poly** and others), is a rechargeable battery of lithium-ion technology using a polymer electrolyte instead of a liquid electrolyte. High conductivity semisolid (gel) polymers form this electrolyte. These batteries provide higher specific energy than other lithium battery types and are used in applications where weight is a critical feature, such as mobile devices, radio-controlled aircraft and some electric vehicles.

I had purchased this set of [5 batteries with a charger](https://www.amazon.ca/gp/product/B0795F139D) from Amazon for under $15 and been using them for a [previous project](https://github.com/MecaHumArduino/wemos-water-leak-sensor) without a problem, so that shall be my go to battery for this project as well.

3D PRINTED CASE
==========
No one likes wires hanging around, and so I went ahead looking for a 3D case I can use for this project and luckily found [this one on Thingiverse](https://www.thingiverse.com/thing:2550726) so that's what I'll be using


THE WIRING
==========
The **HC-SR04** Ultrasonic Module has 4 pins: Ground, VCC, Trig and Echo.
The Ground and the VCC pins of the module needs to be connected to the Ground and the 5 volts pins on the Wemos Mini respectively. The trig and echo pins to any Digital I/O pin on the Wemos, in our example we use D5 for Trig and D6 for Echo.
Please follow the diagram bellow:

<img align="center" src="https://github.com/MecaHumArduino/wemos-door-alarm-sensor/blob/main/doc/wiring/wemos-and-hcsr04-wiring.png?raw=true" style="max-width:100%;" height="411">

Wiring source files are included under [wiring folder](https://github.com/MecaHumArduino/wemos-water-leak-sensor/tree/main/doc/wiring)

THE CODE
========

The code within `main.cpp` file is well documented, but I'll try to explain the concepts and ideas behind the code in this section. But first of all, copy the file `secrets_copy.h` to `secrets.h` and edit its content with your details: WiFi credentials, Home Assistant details...

The sketch begins with the creation of a few objects we'll need along the way: `Ticker` that calls repeating actions and which we will use to put the board to sleep and then wake up. `WiFiClient` that we use to connect to Wifi and `PubSubClient` that we use to send data through MQTT

```cpp
Ticker ticker;
WiFiClient espClient;
PubSubClient client(espClient);
```

Then we declare a few variables like the Arduino pins for the sensor reading and power, as long as the sleep duration and number of tries we aim to do while connecting to WiFi because we want to avoid draining the battery trying to connect to WiFi indefinitely.

```cpp
#define sensorPower D7 // Power pin
#define sensorPin A0 // Analog Sensor pins
#define durationSleep 30 // seconds
#define NB_TRYWIFI 20 // WiFi connection retries
```

As you might have noticed, there is no `loop()` function in this sketch, only a `setup()` function, and that's because instead of executing some commands in a loop fashion like most Arduino projects, this board is programmed to go to sleep until its wake up moment arrives, in which case, it will executing everything until `setup()` function before going back to sleep again. Here is how it's programmed:

```cpp
void setup()
{
    // only print debug messages to serial if we're in debug mode
    if (DEBUG == true) {
        Serial.print("Waking up ");
    }

    // Step 1: Set D7 pin as an output pin ready to receive power
    pinMode(sensorPower, OUTPUT); // Set D7 as an OUTPUT
    digitalWrite(sensorPower, LOW);

    ...

    // Step 2: Wake the sensor up & get a reading
    int waterLevel = readSensor();

    ...

    // Step 3: If water is detected then
    if (waterLevel > 1) {
        connectToWiFi(); // 1- connect to WiFi
        connectToHass(); // 2- connect to Home Assistant MQTT broker
        publishAlarmToHass(waterLevel); // 3- publish the water level on the MQTT topic
    }

    // Step 4: Go back to sleep for the next 30 sec
    ESP.deepSleep(durationSleep * 1000000);

}
```

The function that reads the water level is straightforward and well documented as well:

```cpp
int readSensor()
{
    // Step 1 : Turn the sensor ON by providing power to D7 pin
	digitalWrite(sensorPower, HIGH);
    // Step 2 : Wait for a just little bit
	delay(10);
    // Step 3 : Perform the reading
	sensorData = analogRead(sensorPin);

    // Step 4 : Turn the sensor OFF
	digitalWrite(sensorPower, LOW);

  return sensorData;
}
```

Finally
======
All contribution to this project is appreciated
