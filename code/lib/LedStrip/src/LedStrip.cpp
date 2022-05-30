#include <LedStrip.h>

/**
 * Constructor
 * @param redPin Pin die de rode kleur representeert.
 * @param greenPin Pin die de groene kleur representeert.
 * @param bluePin Pin die de blauwe kleur representeert.
 */
LedStrip::LedStrip(int redPin, int greenPin, int bluePin) {
  this->redPin = redPin;
  this->greenPin = greenPin;
  this->bluePin = bluePin;
}

/**
 * Roep deze functie op in de setup() functie van de Arduino.
 */
void LedStrip::setup() {
  pinMode(this->redPin, OUTPUT);
  pinMode(this->greenPin, OUTPUT);
  pinMode(this->bluePin, OUTPUT);
}

/**
 * Roep deze functie op in de loop() functie van de Arduino.
 */
void LedStrip::loop() {
  // Blinking
  if (this->blink) {
    // Stop met blinken wanneer de duur is bereikt.
    if (millis() - this->dStart > this->dDuration) {
      this->blink = false;
      this->setColor(this->dDoneColor);
    }

    // Laat de leds blinken.
    else if (this->dLast + this->dSpeed < millis()) {
      this->dLast = millis();
      if (currentColor == COLOR_OFF) {
        this->setColor(this->dColor);
      } else {
        this->setColor(COLOR_OFF);
      }
    }
  }

  // Temporary
  if (this->temporary) {
    // Zet de kleur terug naar de oude kleur wanneer de duur is bereikt.
    if (millis() - this->dStart > this->dDuration) {
      this->temporary = false;
      this->setColor(this->dDoneColor);
    }
  }
}

/**
 * Zet de kleur van de LED strip.
 */
void LedStrip::setColor(int color) {
  if (color == COLOR_OFF) {
    digitalWrite(this->redPin, LED_OFF);
    digitalWrite(this->greenPin, LED_OFF);
    digitalWrite(this->bluePin, LED_OFF);
  } else {
    digitalWrite(this->redPin, color == COLOR_RED ? LED_ON : LED_OFF);
    digitalWrite(this->greenPin, color == COLOR_GREEN ? LED_ON : LED_OFF);
    digitalWrite(this->bluePin, color == COLOR_BLUE ? LED_ON : LED_OFF);
  }

  currentColor = color;
}

/**
 * Zet de kleur van de LED strip na een bepaalde duration.
 * @param color De kleur die moet blinken.
 * @param duration De duur in milliseconden.
 * @param speed De tijd tussen het veranderen van de kleur en uitgeschakeld.
 * @param doneColor De kleur die moet ingesteld worden na de duration.
 */
void LedStrip::setBlinkColor(int color, unsigned long duration, unsigned long speed, int doneColor) {
  this->blink = true;
  this->dStart = millis();
  this->dLast = millis();
  this->dColor = color;
  this->dSpeed = speed;
  this->dDuration = duration;
  this->dDoneColor = doneColor;
}

/**
 * Zet de kleur van de LED strip naar een waarde voor een bepaalde duration.
 * @param color De kleur die moet blinken.
 * @param duration De duur in milliseconden.
 * @param speed De tijd tussen het veranderen van de kleur en uitgeschakeld.
 * @param doneColor De kleur die moet ingesteld worden na de duration.
 */
void LedStrip::setTemporaryColor(int color, unsigned long duration, int doneColor = COLOR_OFF) {
  this->temporary = true;
  this->dStart = millis();
  this->dColor = color;
  this->dDuration = duration;
  this->dDoneColor = doneColor;
  this->setColor(color);
}