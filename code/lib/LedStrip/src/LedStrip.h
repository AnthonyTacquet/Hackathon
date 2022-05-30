#ifndef LedStrip_h
#define LedStrip_h

#define COLOR_OFF 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_BLUE 3

#define LED_ON LOW
#define LED_OFF HIGH

#include <Arduino.h>

typedef void (*Function) ();

class LedStrip {
  public:
    /**
     * Constructor
     * @param redPin Pin die de rode kleur representeert.
     * @param greenPin Pin die de groene kleur representeert.
     * @param bluePin Pin die de blauwe kleur representeert.
     */
    LedStrip(int redPin, int greenPin, int bluePin);

    /**
     * Roep deze functie op in de setup() functie van de Arduino.
     */
    void setup();

    /**
     * Roep deze functie op in de loop() functie van de Arduino.
     */
    void loop();

    /**
     * Zet de kleur van de LED strip.
     * @param color De kleur die moet worden gezet.
     */
    void setColor(int color);

    /**
     * Zet de kleur van de LED strip met blink voor een bepaalde duration.
     * @param color De kleur die moet blinken.
     * @param duration De duur in milliseconden.
     * @param speed De tijd tussen het veranderen van de kleur en uitgeschakeld.
     * @param doneColor De kleur die moet ingesteld worden na de duration.
     */
    void setBlinkColor(int color, unsigned long duration, unsigned long speed, int doneColor = COLOR_OFF);

    /**
     * Zet de kleur van de LED strip naar een waarde voor een bepaalde duration.
     * @param color De kleur die moet blinken.
     * @param duration De duur in milliseconden.
     * @param speed De tijd tussen het veranderen van de kleur en uitgeschakeld.
     * @param doneColor De kleur die moet ingesteld worden na de duration.
     */
    void setTemporaryColor(int color, unsigned long duration, int doneColor = COLOR_OFF);
  private:
    int redPin;
    int greenPin;
    int bluePin;
    int currentColor;

    bool blink;
    bool temporary;
    unsigned long dStart;
    unsigned long dLast;
    unsigned long dSpeed;
    unsigned long dDuration;
    int dColor;
    int dDoneColor;
};

#endif