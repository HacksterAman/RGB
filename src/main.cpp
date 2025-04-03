#include <Arduino.h>

#define BUZZER 16 // D0
#define LED1_R 14 // D5
#define LED1_G 4  // D2
#define LED1_B 5  // D1
#define LED2_R 12 // D6
#define LED2_G 15 // D8
#define LED2_B 13 // D7

int spd = 50;
int eff = 2;
const int MAX_DELAY = 100;
const int MIN_DELAY = 1;
unsigned long prevMillis = 0;

// Variables for effect 1
byte hue = 0;
byte saturation = 255;
byte value = 255;
int r = 255, g = 0, b = 0;      // Start with red for LED1
int r2 = 127, g2 = 127, b2 = 0; // Start with yellow (r+g) for LED2

// Variable for police siren effect
bool sirenState = false;
int sirenBrightness = 0;
int fadeAmount = 15;
bool fadeDirection = true; // true = getting brighter, false = getting dimmer
int blinkPhase = 0;        // 0-7 for different phases of the blink pattern
unsigned long lastPhaseChange = 0;
int sirenPattern = 0;                // 0=double-flash, 1=rotate, 2=blink, 3=strobe
bool autoChangeSiren = true;         // Auto-cycle through patterns by default
unsigned long lastPatternChange = 0; // Timer for auto pattern changes

// Keep track of current LED values
int currentLED1_R = 0, currentLED1_G = 0, currentLED1_B = 0;
int currentLED2_R = 0, currentLED2_G = 0, currentLED2_B = 0;

void processCmd(String cmd);
void rainbowCycle(bool wave);
void policeSiren();

// Map 0-255 to 0-1023 for ESP8266 analogWrite
int mapColorForESP(int colorValue)
{
    return map(colorValue, 0, 255, 0, 1023);
}

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
    // Store current values
    currentLED1_R = L1R;
    currentLED1_G = L1G;
    currentLED1_B = L1B;
    currentLED2_R = L2R;
    currentLED2_G = L2G;
    currentLED2_B = L2B;

    analogWrite(LED1_R, mapColorForESP(L1R));
    analogWrite(LED1_G, mapColorForESP(L1G));
    analogWrite(LED1_B, mapColorForESP(L1B));
    analogWrite(LED2_R, mapColorForESP(L2R));
    analogWrite(LED2_G, mapColorForESP(L2G));
    analogWrite(LED2_B, mapColorForESP(L2B));
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
        // Neon effect
        rainbowCycle(false);
        break;
    case 2:
        // wave effect
        rainbowCycle(true);
        break;
    case 3:
        // Police siren effect - multiple patterns
        policeSiren();
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        break;
    default:
        break;
    }
}

// Rainbow cycle effect - cycles through all colors with offset for LED2
void rainbowCycle(bool wave)
{
    // Calculate delay based on speed setting (0-100%)
    // Higher speed value = faster color changes
    // spd=0 → MAX_DELAY, spd=100 → MIN_DELAY
    if (millis() - prevMillis >= map(100 - spd, 0, 100, MIN_DELAY, MAX_DELAY))
    {
        prevMillis = millis();

        // RGB color wheel cycling algorithm for LED1
        // Smoothly transitions through the color wheel:
        // Red → Yellow → Green → Cyan → Blue → Magenta → Red
        if (r > 0 && b == 0)
        {
            r--;
            g++; // Red to Yellow to Green
        }
        if (g > 0 && r == 0)
        {
            g--;
            b++; // Green to Cyan to Blue
        }
        if (b > 0 && g == 0)
        {
            b--;
            r++; // Blue to Magenta to Red
        }

        // Update LED2 with the same cycling logic
        if (r2 > 0 && b2 == 0)
        {
            r2--;
            g2++;
        }
        if (g2 > 0 && r2 == 0)
        {
            g2--;
            b2++;
        }
        if (b2 > 0 && g2 == 0)
        {
            b2--;
            r2++;
        }

        // Set both LEDs with their respective colors (offset effect)
        if (wave == true)
            setColor(r, g, b, r2, g2, b2);
        else
            setColor(r, g, b, r, g, b);
    }
}

// Original double-flash pattern
void policeDoubleFlash(unsigned long currentMillis)
{
    // Phase timing control - realistic pattern
    if (currentMillis - lastPhaseChange >= map(100 - spd, 0, 100, 50, 250))
    {
        // Special rapid double-flash handling
        if ((blinkPhase == 1 && sirenBrightness == 0) ||
            (blinkPhase == 3 && sirenBrightness == 0) ||
            (blinkPhase == 5 && sirenBrightness == 0) ||
            (blinkPhase == 7 && sirenBrightness == 0))
        {
            lastPhaseChange = currentMillis;
            blinkPhase++;
            if (blinkPhase > 7)
                blinkPhase = 0;
        }
    }

    // Update more frequently for smoother fading
    if (currentMillis - prevMillis >= map(100 - spd, 0, 100, 5, 20))
    {
        prevMillis = currentMillis;

        // Handle different phases of the siren effect
        switch (blinkPhase)
        {
        case 0: // LED1 red flash 1
            if (fadeDirection)
            {
                sirenBrightness += fadeAmount * 2; // Fast rise
                if (sirenBrightness >= 255)
                {
                    sirenBrightness = 255;
                    fadeDirection = false; // Start fading out
                }
            }
            else
            {
                sirenBrightness -= fadeAmount; // Normal fade
                if (sirenBrightness <= 0)
                {
                    sirenBrightness = 0;
                    blinkPhase = 1;                  // Move to next phase
                    fadeDirection = true;            // Reset fade direction
                    lastPhaseChange = currentMillis; // Mark phase change time
                }
            }
            setColor(sirenBrightness, 0, 0, 0, 0, 0); // LED1 red, LED2 off
            break;

        case 1:                         // Short pause before LED1 red flash 2
            setColor(0, 0, 0, 0, 0, 0); // Both LEDs off
            break;

        case 2: // LED1 red flash 2
            if (fadeDirection)
            {
                sirenBrightness += fadeAmount * 2; // Fast rise
                if (sirenBrightness >= 255)
                {
                    sirenBrightness = 255;
                    fadeDirection = false; // Start fading out
                }
            }
            else
            {
                sirenBrightness -= fadeAmount; // Normal fade
                if (sirenBrightness <= 0)
                {
                    sirenBrightness = 0;
                    blinkPhase = 3;                  // Move to next phase
                    fadeDirection = true;            // Reset fade direction
                    lastPhaseChange = currentMillis; // Mark phase change time
                }
            }
            setColor(sirenBrightness, 0, 0, 0, 0, 0); // LED1 red, LED2 off
            break;

        case 3:                         // Longer pause before switching to LED2
            setColor(0, 0, 0, 0, 0, 0); // Both LEDs off
            break;

        case 4: // LED2 blue flash 1
            if (fadeDirection)
            {
                sirenBrightness += fadeAmount * 2; // Fast rise
                if (sirenBrightness >= 255)
                {
                    sirenBrightness = 255;
                    fadeDirection = false; // Start fading out
                }
            }
            else
            {
                sirenBrightness -= fadeAmount; // Normal fade
                if (sirenBrightness <= 0)
                {
                    sirenBrightness = 0;
                    blinkPhase = 5;                  // Move to next phase
                    fadeDirection = true;            // Reset fade direction
                    lastPhaseChange = currentMillis; // Mark phase change time
                }
            }
            setColor(0, 0, 0, 0, 0, sirenBrightness); // LED1 off, LED2 blue
            break;

        case 5:                         // Short pause before LED2 blue flash 2
            setColor(0, 0, 0, 0, 0, 0); // Both LEDs off
            break;

        case 6: // LED2 blue flash 2
            if (fadeDirection)
            {
                sirenBrightness += fadeAmount * 2; // Fast rise
                if (sirenBrightness >= 255)
                {
                    sirenBrightness = 255;
                    fadeDirection = false; // Start fading out
                }
            }
            else
            {
                sirenBrightness -= fadeAmount; // Normal fade
                if (sirenBrightness <= 0)
                {
                    sirenBrightness = 0;
                    blinkPhase = 7;                  // Move to next phase
                    fadeDirection = true;            // Reset fade direction
                    lastPhaseChange = currentMillis; // Mark phase change time
                }
            }
            setColor(0, 0, 0, 0, 0, sirenBrightness); // LED1 off, LED2 blue
            break;

        case 7:                         // Longer pause before restarting sequence
            setColor(0, 0, 0, 0, 0, 0); // Both LEDs off
            break;
        }

        // Adjust fade amount based on speed
        fadeAmount = map(spd, 0, 100, 5, 40);
    }
}

// Rotating pattern - circular alternating red/blue
void policeRotate(unsigned long currentMillis)
{
    if (currentMillis - prevMillis >= map(100 - spd, 0, 100, 5, 30))
    {
        prevMillis = currentMillis;

        // Cycle through 4 states to create rotation illusion
        blinkPhase = (blinkPhase + 1) % 4;

        switch (blinkPhase)
        {
        case 0: // LED1 red, LED2 off
            setColor(255, 0, 0, 0, 0, 0);
            break;
        case 1: // LED1 off, LED2 blue
            setColor(0, 0, 0, 0, 0, 255);
            break;
        case 2: // LED1 blue, LED2 off
            setColor(0, 0, 255, 0, 0, 0);
            break;
        case 3: // LED1 off, LED2 red
            setColor(0, 0, 0, 255, 0, 0);
            break;
        }
    }
}

// Simple alternating blink pattern
void policeBlink(unsigned long currentMillis)
{
    if (currentMillis - prevMillis >= map(100 - spd, 0, 100, 50, 250))
    {
        prevMillis = currentMillis;

        // Toggle state
        sirenState = !sirenState;

        if (sirenState)
        {
            // Both LEDs on - LED1 red, LED2 blue
            setColor(255, 0, 0, 0, 0, 255);
        }
        else
        {
            // Both LEDs off
            setColor(0, 0, 0, 0, 0, 0);
        }
    }
}

// Fast strobe effect
void policeStrobe(unsigned long currentMillis)
{
    if (currentMillis - prevMillis >= map(100 - spd, 0, 100, 10, 80))
    {
        prevMillis = currentMillis;

        // Increment phase
        blinkPhase = (blinkPhase + 1) % 6;

        switch (blinkPhase)
        {
        case 0: // Both LEDs white flash
            setColor(255, 255, 255, 255, 255, 255);
            break;
        case 1: // Off
            setColor(0, 0, 0, 0, 0, 0);
            break;
        case 2: // Both LEDs white flash
            setColor(255, 255, 255, 255, 255, 255);
            break;
        case 3: // Off
            setColor(0, 0, 0, 0, 0, 0);
            break;
        case 4: // LED1 red, LED2 blue
            setColor(255, 0, 0, 0, 0, 255);
            break;
        case 5: // Off
            setColor(0, 0, 0, 0, 0, 0);
            break;
        }
    }
}

// Police siren effect - multiple patterns
void policeSiren()
{
    unsigned long currentMillis = millis();

    // Auto change patterns if enabled
    if (autoChangeSiren && (currentMillis - lastPatternChange >= 5000))
    { // Change every 5 seconds
        lastPatternChange = currentMillis;
        sirenPattern = (sirenPattern + 1) % 4; // Cycle through all 4 patterns

        // Reset phases when changing patterns
        blinkPhase = 0;
        sirenState = false;
        sirenBrightness = 0;
        fadeDirection = true;

        // Display current pattern on serial
        Serial.print("Auto-switching to pattern: ");
        switch (sirenPattern)
        {
        case 0:
            Serial.println("Double Flash");
            break;
        case 1:
            Serial.println("Rotate");
            break;
        case 2:
            Serial.println("Blink");
            break;
        case 3:
            Serial.println("Strobe");
            break;
        }
    }

    // Select the active pattern
    switch (sirenPattern)
    {
    case 0:
        policeDoubleFlash(currentMillis); // Original double-flash pattern
        break;
    case 1:
        policeRotate(currentMillis); // Rotating pattern
        break;
    case 2:
        policeBlink(currentMillis); // Simple blink pattern
        break;
    case 3:
        policeStrobe(currentMillis); // Fast strobe effect
        break;
    default:
        policeDoubleFlash(currentMillis); // Default to double-flash
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
        Serial.print("Effect set to: ");
        Serial.println(eff);
    }
    else if (cmd.startsWith("speed"))
    {
        spd = constrain(cmd.substring(6).toInt(), 0, 100);
        Serial.print("Speed set to: ");
        Serial.println(spd);
    }
    else if (cmd.startsWith("color"))
    {
        // Format: color255000000 (RGB values)
        int R = cmd.substring(5, 8).toInt();
        int G = cmd.substring(8, 11).toInt();
        int B = cmd.substring(11, 14).toInt();

        // Set the same color for both LEDs
        setColor(R, G, B, R, G, B);
        eff = 0; // Disable effects when using static color

        Serial.print("Static color set to RGB(");
        Serial.print(R);
        Serial.print(",");
        Serial.print(G);
        Serial.print(",");
        Serial.print(B);
        Serial.println(")");
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

        Serial.println("Static colors set for both LEDs");
    }
    else
    {
        // Print help menu if command not recognized
        Serial.println("\n--- RGB LED Controller ---");
        Serial.println("Available commands:");
        Serial.println("effect[1-3] - Set effect (1=Rainbow, 2=Same as 1, 3=Police)");
        Serial.println("speed[0-100] - Set speed (0=slowest, 100=fastest)");
        Serial.println("color[RRRGGGBBB] - Set same color for both LEDs");
        Serial.println("static[RRRGGGBBB][RRRGGGBBB] - Set different colors for each LED");
        Serial.println("---------------------------\n");
    }
}