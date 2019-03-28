// -------------------------------------------------------------------------
// This project sends DHT11 temperature and humidity through Sigfox network
//
// Created: 16.05.2017
// Author: Antoine de Chassey
// Code: https://github.com/AntoinedeChassey/MKRFOX1200_weather_station
// --------------------------------------------------------------------------

#include "SigFox.h"
#include "ArduinoLowPower.h"

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

#define DEBUG true              // Set DEBUG to false to disable serial prints
#define SLEEPTIME 60 * 60 * 1000 // Set the delay to 15 minutes (15 min x 60 seconds x 1000 milliseconds)

#define UINT16_t_MAX 65536
#define INT16_t_MAX UINT16_t_MAX / 2

int trigPin = 11; // Trigger
int echoPin = 12; // Echo
long duration, distance;

typedef struct __attribute__((packed)) sigfox_message
{
    int16_t moduleTemperature;
    uint16_t distance;
    uint8_t lastMessageStatus;
} SigfoxMessage;

// Stub for message which will be sent
SigfoxMessage msg;

void setup()
{
    if (DEBUG)
    {
        // We are using Serial1 instead than Serial because we are going in standby
        // and the USB port could get confused during wakeup. To read the DEBUG prints,
        // connect pins 13-14 (TX-RX) to a 3.3V USB-to-serial converter
        Serial1.begin(115200);
        while (!Serial1)
        {
        }
    }

    if (!SigFox.begin())
    {
        // Something is really wrong, try rebooting
        // Reboot is useful if we are powering the board using an unreliable power source
        // (eg. solar panels or other energy harvesting methods)
        reboot();
    }

    // Send module to standby until we need to send a message
    SigFox.end();

    if (DEBUG)
    {
        // Enable DEBUG prints and LED indication if we are testing
        SigFox.debug();
    }

    //Define inputs and outputs
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void loop()
{
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);

    // Convert the time into a distance
    distance = (duration / 2) / 29.1; // Divide by 29.1 or multiply by 0.0343

    // msg.dhtTemperature = convertoFloatToInt16(t, 60, -60);
    msg.distance = convertoFloatToUInt16(distance, 600);

    if (DEBUG)
    {
        Serial.print("Distance: ");
        Serial.print(distance);
    }
    // Start the module
    SigFox.begin();
    // Wait at least 30ms after first configuration (100ms before)
    delay(100);

    // We can only read the module temperature before SigFox.end()
float    t = SigFox.internalTemperature();
    msg.moduleTemperature = convertoFloatToInt16(t, 60, -60);

    // Clears all pending interrupts
    SigFox.status();
    delay(1);

    SigFox.beginPacket();
    SigFox.write((uint8_t *)&msg, 12);

    msg.lastMessageStatus = SigFox.endPacket();

    SigFox.end();
    //Sleep for 15 minutes
    LowPower.sleep(SLEEPTIME);
}

void reboot()
{
    NVIC_SystemReset();
    while (1)
        ;
}

int16_t convertoFloatToInt16(float value, long max, long min)
{
    float conversionFactor = (float)(INT16_t_MAX) / (float)(max - min);
    return (int16_t)(value * conversionFactor);
}

uint16_t convertoFloatToUInt16(float value, long max)
{
    float conversionFactor = (float)(UINT16_t_MAX) / (float)(max);
    return (uint16_t)(value * conversionFactor);
}
