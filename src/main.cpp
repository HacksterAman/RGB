#include <Arduino.h>

#define BUZZER 16 // D0
#define LED1_R 14 // D5
#define LED1_G 4  // D2
#define LED1_B 5  // D1
#define LED2_R 12 // D6
#define LED2_G 15 // D8
#define LED2_B 13 // D7

// Calibration factors (adjust based on your LEDs)
float cal_LED1_R = 1.0;
float cal_LED1_G = 0.8;
float cal_LED1_B = 0.9;
float cal_LED2_R = 1.0;
float cal_LED2_G = 0.85;
float cal_LED2_B = 0.95;

// Persistent state
int speedDelay = 10;   // Default delay in milliseconds
int currentEffect = 0; // Default: 0 (off/static color), 1-9 for effects

void processCommand(String cmd);

void setup()
{
    Serial.begin(9600);
    pinMode(BUZZER, OUTPUT);
    pinMode(LED1_R, OUTPUT);
    pinMode(LED1_G, OUTPUT);
    pinMode(LED1_B, OUTPUT);
    pinMode(LED2_R, OUTPUT);
    pinMode(LED2_G, OUTPUT);
    pinMode(LED2_B, OUTPUT);
}

void setColor(int r1, int g1, int b1, int r2, int g2, int b2)
{
    int adj_r1 = min(1023, int(r1 * cal_LED1_R));
    int adj_g1 = min(1023, int(g1 * cal_LED1_G));
    int adj_b1 = min(1023, int(b1 * cal_LED1_B));
    int adj_r2 = min(1023, int(r2 * cal_LED2_R));
    int adj_g2 = min(1023, int(g2 * cal_LED2_G));
    int adj_b2 = min(1023, int(b2 * cal_LED2_B));

    analogWrite(LED1_R, adj_r1);
    analogWrite(LED1_G, adj_g1);
    analogWrite(LED1_B, adj_b1);
    analogWrite(LED2_R, adj_r2);
    analogWrite(LED2_G, adj_g2);
    analogWrite(LED2_B, adj_b2);
}

// Effects (modified to loop or be non-blocking where needed)
void staticGlow()
{
    setColor(1023, 0, 0, 0, 0, 1023); // Persistent static color
}

void breathingEffect()
{
    static int i = 0;
    static bool increasing = true;
    if (increasing)
    {
        setColor(i, 0, 1023 - i, 1023 - i, 0, i);
        i += 10;
        if (i > 1023)
            increasing = false;
    }
    else
    {
        setColor(i, 0, 1023 - i, 1023 - i, 0, i);
        i -= 10;
        if (i < 0)
            increasing = true;
    }
    delay(speedDelay);
}

void oppositePulse()
{
    static int i = 0;
    setColor(i, 1023 - i, 0, 1023 - i, i, 0);
    i = (i + 10) % 1024; // Loops from 0 to 1023
    delay(speedDelay);
}

void rainbowCycle()
{
    static int i = 0;
    if (i < 1023)
    {
        setColor(i, 1023 - i, 0, 0, i, 1023 - i);
    }
    else
    {
        setColor(1023 - (i - 1023), 0, i - 1023, 1023 - (i - 1023), 1023, 0);
    }
    i = (i + 10) % 2046; // Double range for full cycle
    delay(speedDelay);
}

void waveMotion()
{
    static int i = 0;
    if (i < 1023)
    {
        setColor(i, 0, 1023 - i, 0, i, 1023 - i);
    }
    else
    {
        setColor(1023 - (i - 1023), 1023, 0, 1023 - (i - 1023), 0, 1023);
    }
    i = (i + 10) % 2046;
    delay(speedDelay);
}

void colorChase()
{
    static int step = 0;
    int colors[][3] = {{1023, 0, 0}, {0, 1023, 0}, {0, 0, 1023}, {1023, 1023, 0}};
    int numColors = 4;
    int current = step / 10 % numColors;
    int next = (current + 1) % numColors;
    setColor(colors[next][0], colors[next][1], colors[next][2],
             colors[current][0], colors[current][1], colors[current][2]);
    step = (step + 1) % (numColors * 10); // Slower transition
    delay(speedDelay);
}

void randomFlicker()
{
    int r1 = random(0, 1024);
    int g1 = random(0, 1024);
    int b1 = random(0, 1024);
    int r2 = random(0, 1024);
    int g2 = random(0, 1024);
    int b2 = random(0, 1024);
    setColor(r1, g1, b1, r2, g2, b2);
    delay(speedDelay);
}

void colorWipe()
{
    static int i = 0;
    int r = map(i, 0, 100, 1023, 0);
    int g = map(i, 0, 100, 0, 0);
    int b = map(i, 0, 100, 0, 1023);
    setColor(r, g, b, r, g, b);
    i = (i + 1) % 101; // Loops from red to blue
    delay(speedDelay);
}

void strobe()
{
    static bool on = false;
    if (on)
    {
        setColor(1023, 1023, 1023, 1023, 1023, 1023); // White
    }
    else
    {
        setColor(0, 0, 0, 0, 0, 0); // Off
    }
    on = !on;
    delay(speedDelay);
}

String command = "";

void loop()
{
    // Handle serial input
    if (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            processCommand(command);
            command = "";
        }
        else
        {
            command += c;
        }
    }

    // Run the current effect persistently
    switch (currentEffect)
    {
    case 1:
        staticGlow();
        break;
    case 2:
        breathingEffect();
        break;
    case 3:
        oppositePulse();
        break;
    case 4:
        rainbowCycle();
        break;
    case 5:
        waveMotion();
        break;
    case 6:
        colorChase();
        break;
    case 7:
        randomFlicker();
        break;
    case 8:
        colorWipe();
        break;
    case 9:
        strobe();
        break;
    default:
        break; // 0 or invalid: static color persists
    }
}

void processCommand(String cmd)
{
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);

    if (cmd.startsWith("effect"))
    {
        currentEffect = cmd.substring(6).toInt();
    }
    else if (cmd.startsWith("speed"))
    {
        speedDelay = cmd.substring(6).toInt();
    }
    else if (cmd.startsWith("color"))
    {
        int r = cmd.substring(6, 9).toInt() * 4;
        int g = cmd.substring(10, 13).toInt() * 4;
        int b = cmd.substring(14, 17).toInt() * 4;
        setColor(r, g, b, r, g, b);
        currentEffect = 0; // Static color mode
    }
}