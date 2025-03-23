#include <Arduino.h>

#define BUZZER 16 // D0

#define LED1_R 14 // D5
#define LED1_G 4  // D2
#define LED1_B 5  // D1

#define LED2_R 12 // D6
#define LED2_G 15 // D8
#define LED2_B 13 // D7

void setup()
{
    pinMode(BUZZER, OUTPUT);

    pinMode(LED1_R, OUTPUT);
    pinMode(LED1_G, OUTPUT);
    pinMode(LED1_B, OUTPUT);

    pinMode(LED2_R, OUTPUT);
    pinMode(LED2_G, OUTPUT);
    pinMode(LED2_B, OUTPUT);
}

// Set RGB colors for both LEDs
void setColor(int r1, int g1, int b1, int r2, int g2, int b2)
{
    analogWrite(LED1_R, r1);
    analogWrite(LED1_G, g1);
    analogWrite(LED1_B, b1);

    analogWrite(LED2_R, r2);
    analogWrite(LED2_G, g2);
    analogWrite(LED2_B, b2);
}

// Static Glow: Both LEDs show a steady color
void staticGlow()
{
    setColor(1023, 0, 0, 0, 0, 1023); // LED1 Red, LED2 Blue
    delay(2000);
}

// Breathing Effect: Smooth fade in and out
void breathingEffect()
{
    for (int i = 0; i <= 1023; i += 10)
    {
        setColor(i, 0, 1023 - i, 1023 - i, 0, i);
        delay(10);
    }
    for (int i = 1023; i >= 0; i -= 10)
    {
        setColor(i, 0, 1023 - i, 1023 - i, 0, i);
        delay(10);
    }
}

// Opposite Pulse: One LED brightens while the other dims
void oppositePulse()
{
    for (int i = 0; i <= 1023; i += 10)
    {
        setColor(i, 1023 - i, 0, 1023 - i, i, 0);
        delay(10);
    }
}

// Rainbow Cycle: Smooth RGB transitions
void rainbowCycle()
{
    for (int i = 0; i < 1023; i += 10)
    {
        setColor(i, 1023 - i, 0, 0, i, 1023 - i);
        delay(10);
    }
    for (int i = 1023; i > 0; i -= 10)
    {
        setColor(i, 0, 1023 - i, 1023 - i, 1023, 0);
        delay(10);
    }
}

// Wave Motion: Colors shift between LEDs
void waveMotion()
{
    for (int i = 0; i <= 1023; i += 10)
    {
        setColor(i, 0, 1023 - i, 0, i, 1023 - i);
        delay(10);
    }
    for (int i = 1023; i >= 0; i -= 10)
    {
        setColor(i, 1023, 0, i, 0, 1023);
        delay(10);
    }
}

void loop()
{
    // staticGlow();
    // breathingEffect();
    // oppositePulse();
    // rainbowCycle();
    // waveMotion();
}
