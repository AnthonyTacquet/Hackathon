#include <Arduino.h>
#include <ControlRoomConnection.h>
#include <LedStrip.h>
#include <LedControl.h>
#include <Wire.h>

int switchState1 = 0;
int switchState2 = 0;
int switchState3 = 0;
int switchState4 = 0;

int analogValue = 0;

int laser = 13;
bool again = false;
bool active = false; // false
int time = 0;
int time2 = 0;

//Order: Teresa G. Gant; Jean W. Harmon; Phillip R. Giles; Eric A. Fenster

ControlRoomConnection controlRoom(Serial);
LedStrip ledStrip(9, 10, 11);
// 9 = GREEN
// 10 = RED
// 11 = BLUE

void readInputs(){
  // Reads te state of each switch repeatedly
  switchState1 = !digitalRead(3); //2
  switchState2 = !digitalRead(5); //3
  switchState3 = !digitalRead(4); //4
  switchState4 = !digitalRead(2); //5
  // DON'T FORGET TO RANDOMIZE

  analogValue = analogRead(A0);
}

void setup() {
  controlRoom.setup();
  ledStrip.setup();

  pinMode(2, INPUT_PULLUP); //Switch 1
  pinMode(3, INPUT_PULLUP); //Switch 2
  pinMode(4, INPUT_PULLUP); //Switch 3
  pinMode(5, INPUT_PULLUP); //Switch 4
  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);

  // Wanneer een reset signaal ontvangen wordt van de controle kamer.
  controlRoom.onReset([]() {
    //Serial.println("RESET");
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
  controlRoom.playSound(1); // play sound "try that again"
}

void loop(){
  controlRoom.loop();
  ledStrip.loop();
  
  readInputs();

  //Serial.println(switchState1);

  //Serial.println(analogValue);

  if (!active)
  {
    digitalWrite(laser, LOW);
    time2 = 0;
    time = 0;
  }
  
  if(!again && active){
    if(switchState1 == HIGH && switchState2 == HIGH && switchState3 == HIGH && switchState4 == HIGH ){
      digitalWrite(laser, HIGH);
      ledStrip.setColor(COLOR_GREEN); // set colour green
      if(time2 == 0){
        controlRoom.playSound(9); // light saber sound
        time2++;
      }

      if (analogValue > 300)
      {
        controlRoom.playSound(10); // applause sound
        //Serial.println("OK");
        controlRoom.complete();
      }    
    } else {
      digitalWrite(laser, LOW);
      ledStrip.setColor(COLOR_BLUE); // set colour blue
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

  } else if (again && active){
    if (time == 0)
    {
      controlRoom.playSound(8); // play sound "try that again"
      time ++;
    }
    if(switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW){
      again = false; 
      time = 0;
    } else {
      //ledStrip.setColor(COLOR_RED); // set colour red
      ledStrip.setColor(COLOR_RED);
    }
  }
}