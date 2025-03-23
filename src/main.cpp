#include <Arduino.h>

#define BUZZER 16 // D0
#define LED1_R 14 // D5
#define LED1_G 4  // D2
#define LED1_B 5  // D1
#define LED2_R 12 // D6
#define LED2_G 15 // D8
#define LED2_B 13 // D7

int spd = 50;
int eff = 0;
const int MAX_DELAY = 100;
const int MIN_DELAY = 1;
unsigned long prevMillis = 0;

void processCmd(String cmd);

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

void setColor(int L1R, int L1G, int L1B, int L2R, int L2G, int L2B)
{
    analogWrite(LED1_R, L1R);
    analogWrite(LED1_G, L1G);
    analogWrite(LED1_B, L1B);
    analogWrite(LED2_R, L2R);
    analogWrite(LED2_G, L2G);
    analogWrite(LED2_B, L2B);
}

void breathingEffect()
{
    static int i = 0;
    static bool inc = true;
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY);
    if (currMillis - prevMillis >= delayMs)
    {
        if (inc)
        {
            setColor(i, 255 - i, 0, 255 - i, 0, i); // Added green
            i += 2;
            if (i > 255)
                inc = false;
        }
        else
        {
            setColor(i, 255 - i, 0, 255 - i, 0, i);
            i -= 2;
            if (i < 0)
                inc = true;
        }
        prevMillis = currMillis;
    }
}

void oppositePulse()
{
    static int i = 0;
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY);
    if (currMillis - prevMillis >= delayMs)
    {
        setColor(i, 255 - i, (i + 128) % 256, 255 - i, i, (i + 128) % 256); // Added blue
        i = (i + 2) % 256;
        prevMillis = currMillis;
    }
}

void rainbowCycle()
{
    static int i = 0;
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY);
    if (currMillis - prevMillis >= delayMs)
    {
        if (i < 255)
            setColor(i, 255 - i, 0, 0, i, 255 - i);
        else if (i < 510)
            setColor(255 - (i - 255), 0, i - 255, 255 - (i - 255), 255, 0);
        else
            setColor(255, 255, 255 - (i - 510), 255 - (i - 510), 255, 255); // Added white
        i = (i + 2) % 765;
        prevMillis = currMillis;
    }
}

void waveMotion()
{
    static int i = 0;
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY);
    if (currMillis - prevMillis >= delayMs)
    {
        if (i < 255)
            setColor(i, 0, 255 - i, 0, i, 255 - i);
        else if (i < 510)
            setColor(255 - (i - 255), 255, 0, 255 - (i - 255), 0, 255);
        else
            setColor(255, 255 - (i - 510), 255 - (i - 510), 255 - (i - 510), 255, 255); // Added white
        i = (i + 2) % 765;
        prevMillis = currMillis;
    }
}

void colorChase()
{
    static int step = 0;
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY) * 5;
    if (currMillis - prevMillis >= delayMs)
    {
        int colors[][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {0, 255, 255}, {255, 0, 255}, {255, 255, 255}};
        int numColors = 7;
        int curr = step % numColors;
        int next = (curr + 1) % numColors;
        setColor(colors[next][0], colors[next][1], colors[next][2],
                 colors[curr][0], colors[curr][1], colors[curr][2]);
        step = (step + 1) % numColors;
        prevMillis = currMillis;
    }
}

void randomFlicker()
{
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY);
    if (currMillis - prevMillis >= delayMs)
    {
        setColor(random(0, 256), random(0, 256), random(0, 256),
                 random(0, 256), random(0, 256), random(0, 256));
        prevMillis = currMillis;
    }
}

void colorWipe()
{
    static int i = 0;
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY);
    if (currMillis - prevMillis >= delayMs)
    {
        int r = map(i, 0, 100, 255, 0);
        int g = map(i, 0, 100, 0, 255); // Added green
        int b = map(i, 0, 100, 0, 255);
        setColor(r, g, b, r, g, b);
        i = (i + 1) % 101;
        prevMillis = currMillis;
    }
}

void strobe()
{
    static int step = 0;
    unsigned long currMillis = millis();
    int delayMs = map(spd, 0, 100, MAX_DELAY, MIN_DELAY);
    if (currMillis - prevMillis >= delayMs)
    {
        int colors[][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 255}};
        int numColors = 4;
        if (step % 2 == 0)
        {
            int c = (step / 2) % numColors;
            setColor(colors[c][0], colors[c][1], colors[c][2], colors[c][0], colors[c][1], colors[c][2]);
        }
        else
            setColor(0, 0, 0, 0, 0, 0);
        step++;
        prevMillis = currMillis;
    }
}

String cmd = "";

void loop()
{
    if (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            processCmd(cmd);
            cmd = "";
        }
        else
        {
            cmd += c;
        }
    }

    switch (eff)
    {
    case 1:
        breathingEffect();
        break;
    case 2:
        oppositePulse();
        break;
    case 3:
        rainbowCycle();
        break;
    case 4:
        waveMotion();
        break;
    case 5:
        colorChase();
        break;
    case 6:
        randomFlicker();
        break;
    case 7:
        colorWipe();
        break;
    case 8:
        strobe();
        break;
    default:
        break;
    }
}

void processCmd(String cmd)
{
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);

    if (cmd.startsWith("effect"))
    {
        eff = cmd.substring(6).toInt();
    }
    else if (cmd.startsWith("speed"))
    {
        spd = constrain(cmd.substring(6).toInt(), 0, 100);
    }
    else if (cmd.startsWith("static"))
    {
        int L1R = cmd.substring(7, 10).toInt();
        int L1G = cmd.substring(11, 14).toInt();
        int L1B = cmd.substring(15, 18).toInt();
        int L2R = cmd.substring(19, 22).toInt();
        int L2G = cmd.substring(23, 26).toInt();
        int L2B = cmd.substring(27, 30).toInt();
        setColor(L1R, L1G, L1B, L2R, L2G, L2B);
        eff = 0;
    }
}