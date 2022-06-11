#include <Arduino.h>
#include <Door.h>
#include <LedStrip.h>
#include <ControlRoomConnection.h>

// LED strip
LedStrip ledStrip(2, 3, 4);
// Door
Door door(9);
// Connectie met de controle kamer
ControlRoomConnection controlRoom(Serial);
// Spel is actief
bool active = true;
// Mijn Code
#define select 3
#define enter 4
#define rood 5  //---
#define groen 6 //   |-- Ledstrip kleuren
#define blauw 7 //---
String programma[] = {"SOS", "ICT", "WIN"};
int randomNr;
int tel = 0;
String woord;
#include "FireTimer.h"
FireTimer timerSos;
FireTimer timerSos2;
FireTimer hint;
FireTimer wrong;
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Printen.h"
#include "Morse.h"

void setup()
{
  door.setup();
  ledStrip.setup();
  controlRoom.setup();
  Serial.println("Setup!");
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
    active = true;
    Serial.println("Hallo!"); });
  // Wanneer een verlies signaal ontvangen wordt van de controle kamer.
  controlRoom.onLose([]()
                     {
    ledStrip.setBlinkColor(COLOR_RED, 5000, 200, COLOR_RED);
    active = false;
    
    Serial.println("LOST!"); });
  // Wanneer een win signaal ontvangen wordt van de controle kamer.
  controlRoom.onWin([]()
                    {
    ledStrip.setBlinkColor(COLOR_GREEN, 5000, 200, COLOR_GREEN);
    active = false; });
  // Mijn Code
  // Morse
  pinMode(select, INPUT);
  digitalWrite(select, LOW);
  pinMode(enter, INPUT);
  digitalWrite(enter, LOW);
  pinMode(0, INPUT);
  pinMode(9, OUTPUT);
  randomSeed(analogRead(0));
  randomNr = random(3);
  pinMode(1, INPUT);
  // lcd display
  lcd.init();
  lcd.init();
  lcd.backlight();
  // led
  pinMode(blauw, OUTPUT);
  pinMode(rood, OUTPUT);
  pinMode(groen, OUTPUT);
}

void loop()
{
  door.loop();
  ledStrip.loop();
  controlRoom.loop();
  if (active)
  {
    door.close();
    door.open();
    printe();
    if (randomNr == 0)
    {
      if (fase < 3 || fase >= 6 && fase < 9)
      {
        s();
      }
      if (fase >= 3 && fase < 6)
      {
        o();
      }
      if (fase >= 9)
      {
        wachten();
      }
    }
    else if (randomNr == 1)
    {
      if (fase < 2)
      {
        i();
      }
      if (fase >= 2 && fase < 6)
      {
        c();
      }
      if (fase >= 6 && fase < 7)
      {
        t();
      }
      if (fase >= 7)
      {
        wachten();
      }
    }
    else if (randomNr == 2)
    {
      if (fase < 3)
      {
        w();
      }
      if (fase >= 3 && fase < 5)
      {
        i();
      }
      if (fase >= 5 && fase < 7)
      {
        n();
      }
      if (fase >= 7)
      {
        wachten();
      }
    }
  }
}