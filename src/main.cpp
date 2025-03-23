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

int speedDelay = 10; // Default delay in milliseconds

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

// Existing Effects
void staticGlow()
{
    setColor(1023, 0, 0, 0, 0, 1023);
    delay(2000);
}

void breathingEffect()
{
    for (int i = 0; i <= 1023; i += 10)
    {
        setColor(i, 0, 1023 - i, 1023 - i, 0, i);
        delay(speedDelay);
    }
    for (int i = 1023; i >= 0; i -= 10)
    {
        setColor(i, 0, 1023 - i, 1023 - i, 0, i);
        delay(speedDelay);
    }
}

void oppositePulse()
{
    for (int i = 0; i <= 1023; i += 10)
    {
        setColor(i, 1023 - i, 0, 1023 - i, i, 0);
        delay(speedDelay);
    }
}

void rainbowCycle()
{
    for (int i = 0; i < 1023; i += 10)
    {
        setColor(i, 1023 - i, 0, 0, i, 1023 - i);
        delay(speedDelay);
    }
    for (int i = 1023; i > 0; i -= 10)
    {
        setColor(i, 0, 1023 - i, 1023 - i, 1023, 0);
        delay(speedDelay);
    }
}

void waveMotion()
{
    for (int i = 0; i <= 1023; i += 10)
    {
        setColor(i, 0, 1023 - i, 0, i, 1023 - i);
        delay(speedDelay);
    }
    for (int i = 1023; i >= 0; i -= 10)
    {
        setColor(i, 1023, 0, i, 0, 1023);
        delay(speedDelay);
    }
}

// New Effects
void colorChase()
{
    int colors[][3] = {{1023, 0, 0}, {0, 1023, 0}, {0, 0, 1023}, {1023, 1023, 0}};
    int numColors = 4;
    for (int i = 0; i < numColors; i++)
    {
        setColor(colors[i][0], colors[i][1], colors[i][2], 0, 0, 0);
        delay(speedDelay * 10);
        int nextColor = (i + 1) % numColors;
        setColor(colors[nextColor][0], colors[nextColor][1], colors[nextColor][2],
                 colors[i][0], colors[i][1], colors[i][2]);
        delay(speedDelay * 10);
    }
}

void randomFlicker()
{
    for (int i = 0; i < 100; i++)
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
}

void colorWipe(int r_start, int g_start, int b_start, int r_end, int g_end, int b_end)
{
    for (int i = 0; i <= 100; i++)
    {
        int r = map(i, 0, 100, r_start, r_end);
        int g = map(i, 0, 100, g_start, g_end);
        int b = map(i, 0, 100, b_start, b_end);
        setColor(r, g, b, r, g, b);
        delay(speedDelay);
    }
}

void strobe()
{
    for (int i = 0; i < 20; i++)
    {
        setColor(1023, 1023, 1023, 1023, 1023, 1023);
        delay(speedDelay);
        setColor(0, 0, 0, 0, 0, 0);
        delay(speedDelay);
    }
}

String command = "";

void loop()
{
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
}

void processCommand(String cmd)
{
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);

    if (cmd.startsWith("effect"))
    {
        int effectNum = cmd.substring(6).toInt();
        switch (effectNum)
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
            colorWipe(1023, 0, 0, 0, 0, 1023);
            break;
        case 9:
            strobe();
            break;
        default:
            break;
        }
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
    }
}