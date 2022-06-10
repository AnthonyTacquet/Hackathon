#include <IntervalTimer.h>

/**
 * Constructor
 * @param interval Interval in milliseconden.
 * @param times Het aantal keer dat de callback functie moet worden uitgevoerd.
 * @param callback Callback function om uit te voeren.
 */
IntervalTimer::IntervalTimer(unsigned long interval, int times, CallbackFunction callback) {
  this->interval = interval;
  this->times = times;
  this->callback = callback;
  this->timesLeft = 0;
  this->running = false;
}

/**
 * Roep deze functie op in de setup() functie van de Arduino.
 */
void IntervalTimer::setup() {
  this->timer.begin(this->interval);
} 

/**
 * Roep deze functie op in de loop() functie van de Arduino.
 */
void IntervalTimer::loop() {
  if (this->timer.fire()) {
    if (this->running && this->timesLeft > 0) {
      this->timesLeft = this->timesLeft - 1;
      this->callback(this->timesLeft);

      // Disable timer when timesLeft is 0.
      if (this->timesLeft <= 0) {
        this->running = false;
      }
    }
  }
}

/**
 * Start de interval timer.
 */
void IntervalTimer::start() {
  this->timesLeft = this->times;
  this->running = true;
}

/**
 * Stop de interval timer.
 */
void IntervalTimer::stop() {
  this->running = false;
}