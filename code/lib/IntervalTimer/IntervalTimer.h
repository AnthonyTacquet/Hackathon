#ifndef IntervalTimer_h
#define IntervalTimer_h

#include <FireTimer.h>

typedef void (*CallbackFunction) (int timesLeft);

class IntervalTimer {
  public:
    /**
     * Constructor
     * @param interval Interval in milliseconden.
     * @param times Het aantal keer dat de callback functie moet worden uitgevoerd.
     * @param callback Callback function om uit te voeren.
     */
    IntervalTimer(unsigned long interval, int times, CallbackFunction callback);

    /**
     * Roep deze functie op in de setup() functie van de Arduino.
     */
    void setup(); 

    /**
     * Roep deze functie op in de loop() functie van de Arduino.
     */
    void loop();

    /**
     * Start de interval timer.
     */
    void start();

    /**
     * Stop de interval timer.
     */
    void stop();
  private:
    bool running;
    unsigned long interval;
    int times;
    int timesLeft;
    CallbackFunction callback;
    FireTimer timer;

};

#endif