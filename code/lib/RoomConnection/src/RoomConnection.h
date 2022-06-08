#ifndef RoomConnection_h
#define RoomConnection_h

#include <Arduino.h>

typedef void (*Function) ();
typedef void (*FunctionInt) (int sound);

class RoomConnection {
  public:
    /**
     * Constructor
     * @param name Naam van de kamer
     * @param serial Seriële hardwareverbinding-object
     */
    RoomConnection(String name, HardwareSerial &serial);

    /**
     * Roep deze functie op in de setup() functie van de Arduino.
     */
    void setup();

    /**
     * Roep deze functie op in de loop() functie van de Arduino.
     */
    void loop();

    /**
     * Stuur een "reset" signaal.
     */
    void reset();

    /**
     * Stuur een "start" signaal.
     */
    void start();

    /**
     * Stuur een "lose" signaal.
     */
    void lose();

    /**
     * Stuur een "win" signaal.
     */
    void win();

    /**
     * Event handler wannneer het "complete" signaal wordt ontvangen.
     * @param function Lambda function
     */
    void onComplete(Function function);

    /**
     * Event handler wannneer het "sound" signaal wordt ontvangen.
     * @param function Lambda function
     */
    void onSound(FunctionInt function);
  private:
    String name;
    String buffer;
    HardwareSerial* serial;
    Function onCompleteFunction;
    FunctionInt onSoundFunction;

    /**
     * Schrijf een bericht naar de seriële verbinding.
     * @param message 
     */
    void write(String message);

    /**
     * Lees het volgende bericht van de seriële verbinding.
     */
    String read();
};

#endif