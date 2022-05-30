#ifndef ControlRoomConnection_h
#define ControlRoomConnection_h

#include <Arduino.h>

typedef void (*Function) ();

class ControlRoomConnection {
  public:
    /**
     * Constructor
     * @param serial Seriële hardwareverbinding-object
     */
    ControlRoomConnection(HardwareSerial &serial);

    /**
     * Roep deze functie op in de setup() functie van de Arduino.
     */
    void setup();

    /**
     * Roep deze functie op in de loop() functie van de Arduino.
     */
    void loop();

    /**
     * Speel een MP3-bestand af.
     * @param sound Index van het MP3-bestand
     */
    void playSound(int sound);

    /**
     * Stuur een "complete" signaal.
     */
    void complete();
    
    /**
     * Event handler wannneer het reset signaal wordt ontvangen.
     * @param function Lambda function
     */
    void onReset(Function function);

    /**
     * Event handler wannneer het start signaal wordt ontvangen.
     * @param function Lambda function
     */
    void onStart(Function function);

    /**
     * Event handler wannneer het verlies signaal wordt ontvangen.
     * @param function Lambda function
     */
    void onLose(Function function);

    /**
     * Event handler wannneer het win signaal wordt ontvangen.
     * @param function Lambda function
     */
    void onWin(Function function);
  private:
    HardwareSerial* serial;
    String buffer;
    Function onResetFunction;
    Function onStartFunction;
    Function onLoseFunction;
    Function onWinFunction;

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