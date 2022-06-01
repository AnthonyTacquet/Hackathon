#include <Arduino.h>
#include <ControlRoomConnection.h>
#include <LedStrip.h>
#include <Button.h>
#include <IRremote.h>
#include <FireTimer.h>
#include <LedControl.h>
#include "Coordinate.h"

#define SNAKE_MAX_LENGTH 5
#define SNAKE_BOARD_SIZE 8
#define SNAKE_START_POSITION 4, 0

// LED strip
LedStrip ledStrip(2, 3, 4);

// LED Dot Matrix
LedControl ledMatrix(12, 11, 10, 1);

// IR Remote
IRrecv irReceiver(5);

// Connectie met de controle kamer
ControlRoomConnection controlRoom(Serial);

// Of de kamer actief is.
bool active = true;

// Of de snake gestorven is.
bool dead = false;
long deadTime = 0;

// Of de snake 

// Game loop timer
// 1 tick is 1 seconde.
FireTimer snakeLoopTimer;

// Snake lichaam coördinaten.
Coordinate snakeBody[SNAKE_MAX_LENGTH] = {
  Coordinate(SNAKE_START_POSITION),
  Coordinate(-1, -1),
  Coordinate(-1, -1),
  Coordinate(-1, -1),
  Coordinate(-1, -1)
};

// Coordinaat van het eten.
Coordinate snakeFood = Coordinate(-1, -1);

// Beweegrichting van de snake.
int xDirection = 0;
int yDirection = 1;

// Huidige lengte van de snake.
int snakeLength = 1;

/**
 * Verander de beweegrichting van de snake naar boven. 
 */
void turnUp() {
  xDirection = 0;
  yDirection = -1;
}

/**
 * Verander de beweegrichting van de snake naar beneden. 
 */
void turnDown() {
  xDirection = 0;
  yDirection = 1;
}

/**
 * Verander de beweegrichting van de snake naar links. 
 */
void turnLeft() {
  xDirection = -1;
  yDirection = 0;
}

/**
 * Verander de beweegrichting van de snake naar rechts. 
 */
void turnRight() {
  xDirection = 1;
  yDirection = 0;
}

/**
 * Verplaats het eten van de snake naar een willekeurige locatie op het bord,
 * die niet in de snake zit.
 */
void moveFood() {
  int x = random(0, SNAKE_BOARD_SIZE);
  int y = random(0, SNAKE_BOARD_SIZE);
  snakeFood = Coordinate(x, y);

  for (int i = 0; i < snakeLength; i++) {
    if (snakeBody[i] == snakeFood) {
      moveFood();
      return;
    }
  }
}

/**
 * Reset de snake.
 */
void resetSnake() {
  Serial.println("====> RESET <====");
  snakeLength = 1;
  snakeBody[0] = Coordinate(SNAKE_START_POSITION);
  snakeBody[1] = Coordinate(-1, -1);
  snakeBody[2] = Coordinate(-1, -1);
  snakeBody[3] = Coordinate(-1, -1);
  snakeBody[4] = Coordinate(-1, -1);
  xDirection = 0;
  yDirection = 1;
  moveFood();
}

/**
 * Beweeg de snake een vakje vooruit.
 */
void moveSnake() {
  // Hoofd van de snake.
  int headX = snakeBody[0].x;
  int headY = snakeBody[0].y;

  // Nieuwe coordinaten van het hoofd.
  int newHeadX = headX + xDirection;
  int newHeadY = headY + yDirection;

  // Tail van de snake.
  int tailX = snakeBody[snakeLength - 1].x;
  int tailY = snakeBody[snakeLength - 1].y;

  // Als de nieuwe hoofd-coordinaten buiten het gamebord vallen dan heeft de snake de muur geraakt.
  // Reset de snake.
  if (newHeadX < 0 || newHeadX > SNAKE_BOARD_SIZE - 1 || newHeadY < 0 || newHeadY > SNAKE_BOARD_SIZE - 1) {
    resetSnake();
    return;
  }

  // Als de coordinaten van het eten zijn dan moet het eten worden verwijderd en de lengte van de snake verhoogd.
  if (headX == snakeFood.x && headY == snakeFood.y) {
    // Voeg een stukje toe aan de snake.
    snakeBody[snakeLength].x = tailX;
    snakeBody[snakeLength].y = tailY;
    
    snakeLength++;
    moveFood();

    // Als de lengte van de snake groter is dan de maximum lengte dan is de snake gewonnen.
    if (snakeLength > SNAKE_MAX_LENGTH) {
      resetSnake();
      return;
    }
  }

  // Beweeg de snake een vakje vooruit.
  // Dit gebeurt door de hoofd van de snake te verplaatsen naar de nieuwe positie
  // en alle lichaamselementen te verplaatsen naar de coordinaat voor hun in de lijst.
  // Verplaats lichaam van de snake.
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeBody[i].x = snakeBody[i - 1].x;
    snakeBody[i].y = snakeBody[i - 1].y;
  }

  // Verplaats hoofd van de snake.
  snakeBody[0].x = newHeadX;
  snakeBody[0].y = newHeadY;
}

/**
 * Wanneer de Arduino gestart wordt.
 */
void setup()
{
  controlRoom.setup();

  // Setup de LED Strip
  ledStrip.setup();

  // Setup de LED Matrix
  // Haal het aantal apparaten op dat we hebben "gecreëerd" met Ledcontrol.
  int devices=ledMatrix.getDeviceCount();
  // Alle apparaten initialiseren (in een loop).
  for(int address=0;address<devices;address++) {
    // De MAX72XX IC is in slaapstand modus bij opstarten.
    ledMatrix.shutdown(address,false);
    // Zet de helderheid op een medium niveau.
    ledMatrix.setIntensity(address,8);
    // Maak de dot matrix leeg (clear display).
    ledMatrix.clearDisplay(address);
  }

  // Setup de IR Receiver
  irReceiver.enableIRIn();

  // Start de game loop timer.
  snakeLoopTimer.begin(1000);

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

  // Zet de snake game op de start positie.
  resetSnake();
}

/**
 * Loop die elke milliseconde wordt aangeroepen.
 */
void loop()
{
  controlRoom.loop();
  ledStrip.loop();

  // Decode een mogelijk IR signaal.
  if (irReceiver.decode()) {
    int irResult = irReceiver.decodedIRData.decodedRawData;
    Serial.println(irResult, HEX);

    // UP
    if (irResult == 0x1820 || irResult == 0x1020) {
      turnUp();
    }

    // DOWN
    if (irResult == 0x1821 || irResult == 0x1021) {
      turnDown();
    }

    // LEFT
    if (irResult == 0x1811 || irResult == 0x1011) {
      turnLeft();
    }

    // RIGHT
    if (irResult == 0x1810 || irResult == 0x1010) {
      turnRight();
    }

    irReceiver.resume();
  }

  // Wanneer de game loop uitgevoerd mag worden, beweeg de snake.
  if (snakeLoopTimer.fire()) {
    if (active) {
      moveSnake();
      
      // Print het bord via de serial interface.
      // 'x' -> de snake
      // 'f' -> het eten
      // '.' -> lege ruimte
      /*Serial.print("\n");
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
          Coordinate coord = Coordinate(x, y);

          // Check of de coordinate deel is van de snake.
          bool isSnake = false;
          for (int i = 0; i < SNAKE_MAX_LENGTH; i++) {
            if (snakeBody[i] == coord) {
              isSnake = true;
              break;
            }
          }

          if (isSnake) {
            Serial.print("x");
          } else if (snakeFood == coord) {
            Serial.print("f");
          } else {
            Serial.print(".");
          }
        }
        Serial.print("\n");
      }*/

      for(int row=0;row<8;row++) {
        for(int col=0;col<8;col++) {

          Coordinate coord = Coordinate(col, row);

          // Check of de coordinate deel is van de snake.
          bool isSnake = false;
          for (int i = 0; i < SNAKE_MAX_LENGTH; i++) {
            if (snakeBody[i] == coord) {
              isSnake = true;
              break;
            }
          }
          bool isFood = snakeFood == coord;

          ledMatrix.setLed(0, row, col, isSnake || isFood);
        }
      }
    }
  }
}