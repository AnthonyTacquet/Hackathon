#include "RoomConnection.h"
#include <Arduino.h>
#include <Globals.h>

/**
 * Constructor
 * @param name Naam van de kamer
 * @param serial Seriële hardwareverbinding-object
 */
RoomConnection::RoomConnection(String name, HardwareSerial &serial) {
  this->name = name;
  this->serial = &serial;
}

/**
 * Roep deze functie op in de setup() functie van de Arduino.
 */
void RoomConnection::setup() {
  this->serial->begin(9600);
}

/**
 * Roep deze functie op in de loop() functie van de Arduino.
 */
void RoomConnection::loop() {
  String message = this->read();

  if (message.equals(CMD_COMPLETE)) {
    this->onCompleteFunction();
  }

  if (message.startsWith(CMD_SOUND)) {
    int sound = message.substring(strlen(CMD_SOUND)).toInt();
    this->onSoundFunction(sound);
  }
}

/**
 * Stuur een "reset" signaal.
 */
void RoomConnection::reset() {
  this->write(CMD_RESET);
}

/**
 * Stuur een "start" signaal.
 */
void RoomConnection::start() {
  this->write(CMD_START);
}

/**
 * Stuur een "lost" signaal.
 */
void RoomConnection::lose() {
  this->write(CMD_LOSE);
}

/**
 * Stuur een "won" signaal.
 */
void RoomConnection::win() {
  this->write(CMD_WIN);
}

/**
 * Event handler wannneer het complete signaal wordt ontvangen.
 * @param function Lambda function
 */
void RoomConnection::onComplete(Function function) {
  this->onCompleteFunction = function;
}


/**
 * Schrijf een bericht naar de seriële verbinding.
 * @param message Bericht om te versturen
 */
void RoomConnection::write(String message) {
  this->serial->println(message);
}

/**
 * Lees het volgende bericht van de seriële verbinding.
 */
String RoomConnection::read() {
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