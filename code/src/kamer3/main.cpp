#include <Arduino.h>
#include <ControlRoomConnection.h>
#include <LedStrip.h>
#include <Button.h>
#include <IRremote.h>
#include <FireTimer.h>
#include <IntervalTimer.h>
#include <LedControl.h>
#include <Door.h>
#include "Coordinate.h"

#define SNAKE_MAX_LENGTH 5
#define SNAKE_BOARD_SIZE 8
#define SNAKE_START_POSITION 4, 0

// LED strip
LedStrip ledStrip(5, 4, 6);

// LED Dot Matrix
LedControl ledMatrix(8, 10, 9, 1);

// Door
Door door(11);

// IR Remote
IRrecv irReceiver(7);

// Connectie met de controle kamer
ControlRoomConnection controlRoom(Serial);

// Of de kamer actief is.
bool active = false;

// Snake game status.
bool snakeRunning = false;
bool snakeLost = false;
bool snakeWon = false;

// Snake functies.
// Staat hier apart omdat ze op veel locaties nodig zijn doorheen de code.
void resetSnake();
void startSnake();
void stopSnake();
void renderSnake();
void moveSnake();

// Game loop timer
// 1 tick is 1 seconde.
FireTimer snakeLoopTimer;

// Flash interval timer van het LED scherm.
// Wordt gebruikt om het scherm te flashen als de snake verloren is.
IntervalTimer ledMatrixFlashTimer(200, 16, [](int timesLeft) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ledMatrix.setLed(0, row, col, timesLeft % 2 != 0);
    }
  }

  // Start de snake game opnieuw na het uitvoeren van de flash.
  if (timesLeft == 0) {
    startSnake();
  }
});

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
  snakeWon = false;
  snakeLost = false;
  snakeRunning = false;

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
 * Stop de snake game.
 */
void stopSnake() {
  resetSnake();

  snakeRunning = false;
  snakeLost = false;
  snakeWon = false;
}

/**
 * Start de snake game.
 */
void startSnake() {
  resetSnake();

  snakeRunning = true;
  snakeLost = false;
  snakeWon = false;
}

/**
 * Wanneer de snake game verloren is.
 */
void lostSnake() {
  snakeLost = true;
  snakeRunning = false;
  ledStrip.setBlinkColor(COLOR_RED, 3000, 200, COLOR_BLUE);
  ledMatrixFlashTimer.start();
  controlRoom.playSound(1);
}

/**
 * Wanneer de snake game gewonnen is.
 */
void wonSnake() {
  snakeWon = true;
  snakeRunning = false;
  door.open();
  controlRoom.playSound(3);
  ledStrip.setBlinkColor(COLOR_GREEN, 2000, 200, COLOR_GREEN);
  ledMatrix.clearDisplay(0);

  // Send a complete signal to the control room.
  controlRoom.complete();
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
    lostSnake();
    return;
  }

  // Als de de nieuwe hoofd-coordinaten op een bestaande coordinaat van de snake zit.
  // Dan heeft de snake zichzelf geraakt.
  // Reset de snake.
  for (int i = 0; i < snakeLength; i++) {
    if (snakeBody[i] == Coordinate(newHeadX, newHeadY)) {
      lostSnake();
      return;
    }
  }

  // Als de coordinaten van het eten zijn dan moet het eten worden verwijderd en de lengte van de snake verhoogd.
  if (headX == snakeFood.x && headY == snakeFood.y) {
    // Voeg een stukje toe aan de snake.
    snakeBody[snakeLength].x = tailX;
    snakeBody[snakeLength].y = tailY;
    
    snakeLength++;
    moveFood();

    // Groen voor 500 ms.
    ledStrip.setTemporaryColor(COLOR_GREEN, 500, COLOR_BLUE);

    // Als de lengte van de snake groter is dan de maximum lengte dan is de snake gewonnen.
    if (snakeLength >= SNAKE_MAX_LENGTH) {
      wonSnake();
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
 * Render de snake op de LED matrix.
 */
void renderSnake() {
  for (int x = 0; x < SNAKE_BOARD_SIZE; x++) {
    for (int y = 0; y < SNAKE_BOARD_SIZE; y++) {
      Coordinate coord = Coordinate(x, y);

      // Check of de coordinaat deel is van de snake.
      bool isSnake = false;
      for (int i = 0; i < SNAKE_MAX_LENGTH; i++) {
        if (snakeBody[i] == coord) {
          isSnake = true;
          break;
        }
      }

      // Check of de coordinaat voedsel is.
      bool isFood = snakeFood == coord;

      // Zet het ledje aan als de coordinaat deel is van de snake of eten.
      ledMatrix.setLed(0, x, SNAKE_BOARD_SIZE - y - 1, isSnake || isFood);
    }
  }
}

/**
 * Wanneer de Arduino gestart wordt.
 */
void setup()
{
  controlRoom.setup();

  // Setup de LED Strip
  ledStrip.setup();

  // Setup de deur.
  door.setup();

  // Setup de LED Matrix
  ledMatrix.shutdown(0, false); // Haal de controller uit slaapstand.
  ledMatrix.setIntensity(0, 8); // Zet de helderheid.
  ledMatrix.clearDisplay(0); // Clear de inhoud van het scherm.

  // Setup de IR Receiver
  irReceiver.enableIRIn();

  // Start de game loop timer.
  snakeLoopTimer.begin(700);
  ledMatrixFlashTimer.setup();

  // Wanneer een reset signaal ontvangen wordt van de controle kamer.
  controlRoom.onReset([]() {
    ledStrip.setBlinkColor(COLOR_BLUE, 3000, 200, COLOR_OFF);
    active = false;
    ledMatrix.clearDisplay(0);
    door.close();
  });

  // Wanneer een start signaal ontvangen wordt van de controle kamer.
  controlRoom.onStart([]() {
    ledStrip.setColor(COLOR_BLUE);
    active = true;
    startSnake();
  });

  // Wanneer een verlies signaal ontvangen wordt van de controle kamer.
  controlRoom.onLose([]() {
    ledStrip.setBlinkColor(COLOR_RED, 5000, 200, COLOR_RED);
    active = false;
    stopSnake();
  });

  // Wanneer een win signaal ontvangen wordt van de controle kamer.
  controlRoom.onWin([]() {
    ledStrip.setBlinkColor(COLOR_GREEN, 5000, 200, COLOR_GREEN);
    active = false;
    stopSnake();
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
  door.loop();
  ledStrip.loop();
  ledMatrixFlashTimer.loop();

  // Decode een mogelijk IR signaal.
  if (irReceiver.decode()) {
    int irResult = irReceiver.decodedIRData.decodedRawData;
    // Serial.println(irResult, HEX);

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
    if (snakeRunning) {

      // Move the snake.
      moveSnake();

      // Render the snake.
      renderSnake();      
    }
  }
}