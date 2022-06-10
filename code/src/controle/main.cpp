#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Button.h>
#include <Servo.h>
#include <RoomConnection.h>
#include <TM1637.h>
#include <DFRobotDFPlayerMini.h>
#include <Globals.h>

#define PIN_GAME_BUTTON 12
#define PIN_DISPLAY_DIO 8
#define PIN_DISPLAY_CLK 9
#define PIN_PLAYER_RX 10
#define PIN_PLAYER_TX 11
#define PIN_BUZZER 2

#define GAME_DURATION 600000UL

// Seriële connecties naar alle kamers
RoomConnection room1("kamer1", Serial);
RoomConnection room2("kamer2", Serial1);
RoomConnection room3("kamer3", Serial2);
RoomConnection room4("kamer4", Serial3);

// Display
TM1637 display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);

// Software Serial voor de DFPlayer
SoftwareSerial playerSerial(PIN_PLAYER_RX, PIN_PLAYER_TX);
// DFPlayer
DFRobotDFPlayerMini player;

// Status van de game.
// Mogelijke opties zijn:
// - lost: de speler heeft de game verloren
// - won: de speler heeft de game gewonnen
// - running: het spel loopt
// - idle: het spel is nog niet gestart
// - resetting: het spel is aan het resetten
String gameStatus = "idle";

// De huidige actieve kamer van de game.
int gameRoom = 1;

// Start tijdstip (in milliseconden)
unsigned long gameStart = 0;

// Eind tijdstip (in milliseconden)
unsigned long gameEnd = 0;

// Reset tijdstip (in milliseconden)
unsigned long gameReset = 0;

// De button die de game start/stopt.
Button gameButton(PIN_GAME_BUTTON);

/**
 * Start de game.
 */
void startGame() {
  // Reset de game.
  gameStatus = "running";
  gameRoom = 1;
  gameStart = millis();

  // Start de eerste kamer
  room1.start();
}

/**
 * Reset de game.
 * Reset alle kamers naar hun beginstatus.
 */
void resetGame() {
  gameStatus = "resetting";
  gameRoom = 1;
  gameStart = 0;
  gameEnd = 0;
  gameReset = millis();

  room1.reset();
  room2.reset();
  room3.reset();
  room4.reset();
}

/**
 * Verlies de game.
 * Wanneer de gebruiker het spel heeft verloren door de timer die verloopt.
 */
void loseGame() {
  gameStatus = "lost";
  gameEnd = millis();

  room1.lose();
  room2.lose();
  room3.lose();
  room4.lose();
}

/**
 * Win de game.
 * Wanneer de gebruiker het spel heeft gewonnen door alle kamers te bereiken.
 */
void winGame() {
  gameStatus = "won";
  gameEnd = millis();

  room1.win();
  room2.win();
  room3.win();
  room4.win();
}

/**
 * Idle de game.
 * Wanneer het spel wacht om gestart te worden 
 */
void idleGame() {
  gameStatus = "idle";
  gameReset = 0;
}

/**
 * Wanneer de gebruiker op de game button in de controlekamer drukt.
 */
void onGameButtonClick() {
  // Als de game idle is, start de game.
  if (gameStatus == "idle") {
    startGame();
    return;
  }

  // Als de game running is, stop de game.
  if (gameStatus == "running") {
    loseGame();
    return;
  }
}

/**
 * Display a number on the display.
 * @param number
 */
void displayNumber(int number) {   
  display.display(3, number % 10);   
  display.display(2, number / 10 % 10);   
  display.display(1, number / 100 % 10);   
  display.display(0, number / 1000 % 10);
}

/**
 * Toon de resterende tijd op de display.
 */
void renderDisplay() {
  // Als de game niet aan het lopen is zet de display op 0.
  if (gameStatus != "running") {
    display.display(0, 0);   
    display.display(1, 0);
    display.display(2, 0);
    display.display(3, 0);
    return;
  }

  // Bereken de resterende tijd in milliseconden.
  signed long remainingTime = GAME_DURATION - (millis() - gameStart);

  // Bereken het aantal minuten en seconden.
  unsigned long minutes = remainingTime / 1000 / 60;
  unsigned long seconds = remainingTime / 1000 % 60;

  // Render de tijd
  display.display(0, minutes / 10);   
  display.display(1, minutes % 10);
  display.display(2, seconds / 10);
  display.display(3, seconds % 10);
}

/**
 * Wanneer de Arduino gestart wordt.
 */
void setup()
{
  // Start seriële communicatie met de kamer Arduino's.
  room1.setup();
  room2.setup();
  room3.setup();
  room4.setup();

  // Setup de display
  display.init();
  display.set(7);

  // Setup de DFPlayer
  playerSerial.begin(9600);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!player.begin(playerSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  player.volume(20);

  // Pull-up resistor voor de game button.
  pinMode(PIN_GAME_BUTTON, INPUT_PULLUP);

  // Reset alle kamers initieel
  resetGame();

  // Wanneer een kamer1 opgelost is.
  room1.onComplete([]() {
    if (gameStatus == "running" && gameRoom == 1) {
      gameRoom = 2;
      room2.start();
    }
  });

  // Wanneer een kamer2 opgelost is.
  room2.onComplete([]() {
    if (gameStatus == "running" && gameRoom == 2) {
      gameRoom = 3;
      room3.start();
    }
  });

  // Wanneer een kamer3 opgelost is.
  room3.onComplete([]() {
    if (gameStatus == "running" && gameRoom == 3) {
      gameRoom = 4;
      room4.start();
    }
  });

  // Wanneer een kamer4 opgelost is.
  room4.onComplete([]() {
    if (gameStatus == "running" && gameRoom == 4) {
      winGame();
    }
  });

  // Wanneer de kamers een geluid signaal geven.
  room1.onSound([](int sound) {
    player.playMp3Folder(sound);
  });
  room2.onSound([](int sound) {
    player.playMp3Folder(sound);
  });
  room3.onSound([](int sound) {
    player.playMp3Folder(sound);
  });
  room4.onSound([](int sound) {
    player.playMp3Folder(sound);
  });
}

/**
 * Loop die elke milliseconde wordt aangeroepen.
 */ 
void loop()
{
  // Voer de loop uit voor alle kamer controllers
  room1.loop();
  room2.loop();
  room3.loop();
  room4.loop();

  // Display de huidige tijd
  renderDisplay();

  // Wanneer de gamebutton ingedrukt wordt.
  if (gameButton.pressed()) {
    onGameButtonClick();
  }

  // Als de game running is, check of de timer is verlopen.
  if (gameStatus == "running") {
    if (millis() - gameStart > GAME_DURATION) {
      loseGame();
    }
  }

  // Als de game verloren of gewonnen is, reset de game na 5 seconden.
  if (gameStatus == "lost" || gameStatus == "won") {
    if (millis() - gameEnd > 5000) {
      resetGame();
    }
  }

  // Als de game aan het resetten is, zet de game op idle na 5 seconden.
  if (gameStatus == "resetting") {
    if (millis() - gameReset > 5000) {
      idleGame();
    }
  }
}