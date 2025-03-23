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

void loop()
{
  // Turn on RGB LEDs with different colors
  analogWrite(LED1_R, 512);  // Half brightness Red
  analogWrite(LED1_G, 0);    // Green off
  analogWrite(LED1_B, 1023); // Full brightness Blue

  analogWrite(LED2_R, 1023); // Full brightness Red
  analogWrite(LED2_G, 512);  // Half brightness Green
  analogWrite(LED2_B, 0);    // Blue off

  // Buzzer Beep
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);

  delay(1000); // Wait before next color change
}
