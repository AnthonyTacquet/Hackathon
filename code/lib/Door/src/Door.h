#include <Servo.h>
#include <FireTimer.h>

#define DOOR_OPEN_ROTATION 110
#define DOOR_CLOSED_ROTATION 0
#define DOOR_OPEN_DURATION 2500

class Door {
  public:
    /**
     * Constructor
     * @param doorPin Pin containing the servo motor 
     */
    Door(int doorPin);

    /**
     * Roep deze functie op in de setup() functie van de Arduino.
     */
    void setup();

    /**
     * Roep deze functie op in de loop() functie van de Arduino.
     */
    void loop();

    /**
     * Open the door
     */
    void open();

    /**
     * Close the door
     */
    void close();
    
    bool isOpen;
    bool isMoving;
  private:
    Servo servo;
    FireTimer timer;
    int angle;
    int pin;
};