#include "Door.h"

/**
 * Constructor
 * @param doorPin Pin van de servo motor.
 */
Door::Door(int doorPin) {
  this->pin = doorPin;
  this->angle = DOOR_CLOSED_ROTATION;
  this->isMoving = false;
  this->isOpen = false;
}

/**
 * Open de deur
 */
void Door::open() {
  this->isMoving = true;
  this->isOpen = true;
  this->timer.start();
}

/**
 * Sluit de deur.
 */
void Door::close() {
  this->isMoving = true;
  this->isOpen = false;
  this->timer.start();
}

/**
 * Roep deze functie op in de setup() functie van de Arduino.
 */
void Door::setup() {
  this->servo.attach(this->pin);
  this->servo.write(DOOR_CLOSED_ROTATION);
  this->timer.begin(DOOR_OPEN_DURATION / (DOOR_OPEN_ROTATION - DOOR_CLOSED_ROTATION));
}

/**
 * Roep deze functie op in de loop() functie van de Arduino.
 */
void Door::loop() {
  if (this->timer.fire() && isMoving) {
    // Open rotatie is bereikt.
    if (isOpen && this->angle >= DOOR_OPEN_ROTATION) {
      this->isMoving = false;
      this->angle = DOOR_OPEN_ROTATION;
      return;
    }

    // Gesloten rotatie is bereikt.
    if (!isOpen && this->angle <= DOOR_CLOSED_ROTATION) {
      this->isMoving = false;
      this->angle = DOOR_CLOSED_ROTATION;
      return;
    }

    // Deur openen.
    if (isOpen) {
      this->angle++;
    }

    // Deur sluiten.
    if (!isOpen) {
      this->angle--;
    }

    Serial.println(this->angle);

    this->servo.write(this->angle);
  }
}