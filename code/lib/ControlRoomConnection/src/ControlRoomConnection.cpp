#include "ControlRoomConnection.h"
#include <Arduino.h>
#include <Globals.h>

/**
 * Constructor
 * @param serial Seriële hardwareverbinding-object
 */
ControlRoomConnection::ControlRoomConnection(HardwareSerial& serial) {
  this->serial = &serial;
}

/**
 * Roep deze functie op in de setup() functie van de Arduino.
 */
void ControlRoomConnection::setup() {
  this->serial->begin(9600);
}

/**
 * Roep deze functie op in de loop() functie van de Arduino.
 */
void ControlRoomConnection::loop() {
  // Lees de connectie voor een bericht
  String message = this->read();

  // Controleer of het bericht een commando is
  if (message.equals(CMD_RESET)) {
    this->onResetFunction();
  } else if (message.equals(CMD_START)) {
    this->onStartFunction();
  } else if (message.equals(CMD_LOSE)) {
    this->onLoseFunction();
  } else if (message.equals(CMD_WIN)) {
    this->onWinFunction();
  }
}

/**
 * Stuur een "complete" signaal.
 */
void ControlRoomConnection::complete() {
  this->write(CMD_COMPLETE);
}

/**
 * Speel een audio bestand af.
 */
void ControlRoomConnection::playSound(int sound) {
  this->write(CMD_SOUND + String(sound));
}

/**
 * Event handler wannneer het reset signaal wordt ontvangen.
 * @param function Lambda function
 */
void ControlRoomConnection::onReset(Function function) {
  this->onResetFunction = function;
}

/**
 * Event handler wannneer het start signaal wordt ontvangen.
 * @param function Lambda function
 */
void ControlRoomConnection::onStart(Function function) {
  this->onStartFunction = function;
}

/**
 * Event handler wannneer het verlies signaal wordt ontvangen. 
 * @param function Lambda function
 */
void ControlRoomConnection::onLose(Function function) {
  this->onLoseFunction = function;
}

/**
 * Event handler wannneer het win signaal wordt ontvangen.
 * @param function Lambda function
 */
void ControlRoomConnection::onWin(Function function) {
  this->onWinFunction = function;
}

/**
 * Schrijf een bericht naar de seriële verbinding.
 * @param message Bericht om te versturen
 */
void ControlRoomConnection::write(String message) {
  this->serial->println(message);
}

/**
 * Lees het volgende bericht van de seriële verbinding.
 */
String ControlRoomConnection::read() {
  if (this->serial->available() > 0) {
    char character = this->serial->read();

    // If the character is a newline character, return the buffer
    if (character == '\n') {
      String buffer = this->buffer;
      // Delete carriage return
      buffer.remove(buffer.length() - 1);
      // Clear the buffer
      this->buffer = "";
      return buffer;
    }
    // Add the character to the buffer
    else {
      buffer += character;
    }
  }

  return "_";
}