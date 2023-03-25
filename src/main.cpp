/*
 *  Project 04-hcsr - main.cpp
 *      Electronica Digital - Austral - 2023 - EAM
 *
 *      Using the ultrasonic sensor HC-SR04 to measure distances
 *
 *      ESP32 microcontroller
 */

#include <Arduino.h>

/*
 *  Definitions included in platformio.ini
 *      TRIG_PIN    Trigger output
 *      ECHO_PIN    Echo input
 *      BAUD        Serial baud rate
 */

/*
 *  Another constants
 */

const double sound_speed = 0.0343;    // in cm/microsecs

/*
 *  Private functions
 */

static void
send_trigger( void )
{
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
}

static long
get_pulse( void )
{
    return pulseIn( ECHO_PIN, HIGH );    // in microseconds
}

/*
 *  Public functions
 */

void
setup(void)
{
    pinMode(TRIG_PIN, OUTPUT);          // trigPin as output
    digitalWrite(TRIG_PIN, LOW);        // trigPin to low
    pinMode(ECHO_PIN, INPUT);           // echoPin as input

    Serial.begin(BAUD);
}

void
loop(void)
{
    long duration;
    double distance;

    send_trigger();
    duration = get_pulse();

    distance = duration * sound_speed / 2;
    Serial.printf("Distance = %7.1f cm\n", distance);

    delay(2000);
}
