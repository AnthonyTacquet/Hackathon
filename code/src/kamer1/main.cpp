#include <Arduino.h>
#include "FireTimer.h"
#include <Door.h>
#include <LedStrip.h>
#include <ControlRoomConnection.h>

const int MaxLevel = 8;
int sequence[MaxLevel];
int your_sequence[MaxLevel];
int level = 1;
int velocity = 1000;
bool active = false;
FireTimer msTimer;

// led innput
int ledblue = 2;
int ledyellow = 4;
int ledred = 5;
int ledgreen = 3;
// button input
int buttonblue = 10;
int buttonyellow = 11;
int buttonred = 12;
int buttongreen = 13;
// int buttonstart = 8;

LedStrip ledStrip(9, 8, 7); // ledStrip
Door door(6);               // door

ControlRoomConnection controlRoom(Serial); // Connectie met de controle kamer

void setup()
{

  door.setup();
  ledStrip.setup();
  controlRoom.setup();

  // Wanneer een reset signaal ontvangen wordt van de controle kamer.
  controlRoom.onReset([]()
                      {
    ledStrip.setBlinkColor(COLOR_BLUE, 3000, 200, COLOR_OFF);
    door.close();
    active = false; });

  // Wanneer een start signaal ontvangen wordt van de controle kamer.
  controlRoom.onStart([]()
                      {
    ledStrip.setColor(COLOR_BLUE);
    active = true; });

  // Wanneer een verlies signaal ontvangen wordt van de controle kamer.
  controlRoom.onLose([]()
                     {
    ledStrip.setBlinkColor(COLOR_RED, 5000, 200, COLOR_RED);
    active = false; });

  // Wanneer een win signaal ontvangen wordt van de controle kamer.
  controlRoom.onWin([]()
                    {
    ledStrip.setBlinkColor(COLOR_GREEN, 5000, 200, COLOR_GREEN);
    active = false; });

  msTimer.begin(1000);
  pinMode(buttonblue, INPUT_PULLUP);
  pinMode(buttonyellow, INPUT_PULLUP);
  pinMode(buttonred, INPUT_PULLUP);
  pinMode(buttongreen, INPUT_PULLUP);
  // pinMode(buttonstart, INPUT_PULLUP);
  pinMode(ledblue, OUTPUT);
  pinMode(ledyellow, OUTPUT);
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  digitalWrite(ledblue, LOW);
  digitalWrite(ledyellow, LOW);
  digitalWrite(ledred, LOW);
  digitalWrite(ledgreen, LOW);
}

void show_sequence()
{
  digitalWrite(ledblue, LOW);
  digitalWrite(ledyellow, LOW);
  digitalWrite(ledred, LOW);
  digitalWrite(ledgreen, LOW);
  for (int i = 0; i < level; i++)
  {
    digitalWrite(sequence[i], HIGH);
    msTimer.update(1000);
    digitalWrite(sequence[i], LOW);
    msTimer.update(1000);
  }
}
void wrong_sequence()
{
  level = 1;
  ledStrip.setBlinkColor(COLOR_RED, 5000, 200, COLOR_RED);

}
void right_sequence()
{
  // ledstrip blinks green
  if (level < MaxLevel)
  {
    level++;
  }
   else  {
    controlRoom.complete();
   }
}

void get_sequence()
{
  int flag = 0; // this flag indicates if the sequence is correct
  for (int i = 0; i < level; i++)
  {
    flag = 0;
    while (flag == 0)
    {
      if (digitalRead(buttonblue) == LOW)
      {
        digitalWrite(ledblue, HIGH);
        your_sequence[i] = ledblue;
        flag = 1;
        msTimer.fire();
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(ledblue, LOW);
      }
      if (digitalRead(buttonyellow) == LOW)
      {
        digitalWrite(ledyellow, HIGH);
        your_sequence[i] = ledyellow;
        flag = 1;
        msTimer.update(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(ledyellow, LOW);
      }
      if (digitalRead(buttonred) == LOW)
      {
        digitalWrite(ledred, HIGH);
        your_sequence[i] = ledred;
        flag = 1;
        msTimer.update(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(ledred, LOW);
      }
      if (digitalRead(buttongreen) == LOW)
      {
        digitalWrite(ledgreen, HIGH);
        your_sequence[i] = ledgreen;
        flag = 1;
        msTimer.update(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(ledgreen, LOW);
      }
    }
  }
  right_sequence();
}


void generate_sequence()
{
  randomSeed(millis()); // maken het random
  for (int i = 0; i < MaxLevel; i++)
  {
    sequence[i] = random(2, 6);
  }
  Serial.println(sequence[MaxLevel]);
}

void loop()
{
  if (msTimer.fire())
  {
    if (active)
    {
      generate_sequence();
      if (digitalRead(buttonblue || buttongreen || buttonred || buttonyellow) == LOW || level != 1)
      {
        show_sequence();
        get_sequence(); // wacht op antwoord
      }
    }
  }
}


