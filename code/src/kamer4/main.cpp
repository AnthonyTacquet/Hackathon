#include <Arduino.h>
#include <ControlRoomConnection.h>
#include <LedStrip.h>
#include <Button.h>
#include <IRremote.h>
#include <FireTimer.h>
#include <LedControl.h>

int switchState1 = 0;
int switchState2 = 0;
int switchState3 = 0;
int switchState4 = 0;
int switchState5 = 0;
int laser = 13;
bool again = false;
bool active = true; // false
int time = 0;

ControlRoomConnection controlRoom(Serial);
LedStrip ledStrip(9, 10, 11);
// 9 = GREEN
// 10 = RED
// 11 = BLUE

void setup() {
  controlRoom.setup();
  ledStrip.setup();
  //Serial.begin(9600);

  pinMode(2, INPUT_PULLUP); //Switch 1
  pinMode(3, INPUT_PULLUP); //Switch 2
  pinMode(4, INPUT_PULLUP); //Switch 3
  pinMode(5, INPUT_PULLUP); //Switch 4
  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);
  // pinMode(6, INPUT_PULLUP); Switch 5 - Optional

  // Wanneer een reset signaal ontvangen wordt van de controle kamer.
  controlRoom.onReset([]() {
    ledStrip.setBlinkColor(COLOR_BLUE, 5000, 200, COLOR_OFF);
    active = false;
  });

  // Wanneer een start signaal ontvangen wordt van de controle kamer.
  controlRoom.onStart([]() {
    ledStrip.setColor(COLOR_BLUE);
    active = true;
  });

  // Wanneer een verlies signaal ontvangen wordt van de controle kamer.
  controlRoom.onLose([]() {
    ledStrip.setBlinkColor(COLOR_RED, 5000, 200, COLOR_RED);
    active = false;
  });

  // Wanneer een win signaal ontvangen wordt van de controle kamer.
  controlRoom.onWin([]() {
    ledStrip.setBlinkColor(COLOR_GREEN, 5000, 200, COLOR_GREEN);
    active = false;
  });
}

void loop(){
  controlRoom.loop();
  ledStrip.loop();

  // Reads te state of each switch repeatedly
  switchState1 = !digitalRead(2); 
  switchState2 = !digitalRead(3);
  switchState3 = !digitalRead(4);
  switchState4 = !digitalRead(5);
  //switchState5 = digitalRead(6);
  //String printt = String( switchState1 + " - " + switchState2 + " - " + switchState3 + " - " + switchState4);
  //Serial.println(switchState1);
  int analogValue = analogRead(A0);
  //Serial.println(analogValue);

  if (!active)
  {
    digitalWrite(laser, LOW);
  }
  

  if(!again && active){
   
    if(switchState1 == HIGH && switchState2 == HIGH && switchState3 == HIGH && switchState4 == HIGH ){// && switchState5 == HIGH
      //ledStrip.setBlinkColor(COLOR_GREEN, 5000, 200, COLOR_GREEN);
      digitalWrite(laser, HIGH);
      ledStrip.setColor(COLOR_GREEN);
      if (analogValue > 300)
      {
        controlRoom.complete();
      }
      
    } else {
      digitalWrite(laser, LOW);
      ledStrip.setColor(COLOR_BLUE);
    }
    if(switchState2 == HIGH && switchState1 == LOW){
      again = true;
    }
    if(switchState3 == HIGH && (switchState2 == LOW || switchState3 == LOW)){
      again = true;
    }
    if(switchState4 == HIGH && (switchState3 == LOW || switchState2 == LOW || switchState1 == LOW)){
      again = true;
    }
    /*if(switchState5 == HIGH && (switchState4 == LOW || switchState3 == LOW || switchState2 == LOW || switchState1 == LOW)){
      again = true;
    }*/
  } else if (active){
    // Mistake was made, game will restart when all pins are low
    if (time == 0)
    {
      ledStrip.setBlinkColor(COLOR_RED, 2000, 200, COLOR_RED);
      time ++;
    }
    
    if(switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW && switchState5 == LOW){
      //ledStrip.setBlinkColor(COLOR_RED, 5000, 200, COLOR_RED);
      //ledStrip.setColor(COLOR_BLUE);
      again = false; 
      time = 0;
    }
  }
}