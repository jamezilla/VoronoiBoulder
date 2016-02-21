/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 * The ArduinoGateway prints data received from sensors on the serial link.
 * The gateway accepts input on seral which will be sent out on radio network.
 *
 * The GW code is designed for Arduino Nano 328p / 16MHz
 *
 * Wire connections (OPTIONAL):
 * - Inclusion button should be connected between digital pin 3 and GND
 * - RX/TX/ERR leds need to be connected between +5V (anode) and digital pin 6/5/4 with resistor 270-330R in a series
 *
 * LEDs (OPTIONAL):
 * - To use the feature, uncomment WITH_LEDS_BLINKING in MyConfig.h
 * - RX (green) - blink fast on radio message recieved. In inclusion mode will blink fast only on presentation recieved
 * - TX (yellow) - blink fast on radio message transmitted. In inclusion mode will blink slowly
 * - ERR (red) - fast blink on error during transmission error or recieve crc error
 *
 */

#define NO_PORTB_PINCHANGES

#include <MyTransportNRF24.h>
#include <MyHwATMega328.h>

#include <SPI.h>
#include <MyParserSerial.h>
#include <MySensor.h>
#include <stdarg.h>
#include <PinChangeInt.h>
#include "GatewayUtil.h"

#define RADIO_ERROR_LED_PIN 4  // Error led pin
#define RADIO_RX_LED_PIN    6  // Receive led pin
#define RADIO_TX_LED_PIN    5  // the PCB, on board LED

// NRFRF24L01 radio driver (set low transmit power by default)
MyTransportNRF24 transport(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);

// Hardware profile
MyHwATMega328 hw;

MySensor gw(transport, hw /*, signer*/);

char inputString[MAX_RECEIVE_LENGTH] = "";    // A string to hold incoming commands from serial/ethernet interface
int inputPos = 0;
boolean commandComplete = false;  // whether the string is complete

void parseAndSend(char *commandBuffer);

void output(const char *fmt, ... ) {
  va_list args;
  va_start (args, fmt );
  vsnprintf_P(serialBuffer, MAX_SEND_LENGTH, fmt, args);
  va_end (args);
  Serial.print(serialBuffer);
}

void setup()
{
  gw.begin(incomingMessage, 0, true, 0);

  setupGateway(output);

  // Send startup log message on serial
  serial(PSTR("0;0;%d;0;%d;Gateway startup complete.\n"),  C_INTERNAL, I_GATEWAY_READY);
}

void loop()
{
  gw.process();

  if (commandComplete) {
    // A command was issued from serial interface
    // We will now try to send it to the actuator
    parseAndSend(gw, inputString);
    commandComplete = false;
    inputPos = 0;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inputPos < MAX_RECEIVE_LENGTH - 1 && !commandComplete) {
      if (inChar == '\n') {
        inputString[inputPos] = 0;
        commandComplete = true;
      } else {
        // add it to the inputString:
        inputString[inputPos] = inChar;
        inputPos++;
      }
    } else {
      // Incoming message too long. Throw away
      inputPos = 0;
    }
  }
}

void incomingMessage(const MyMessage &message) {
  // Pass along the message from sensors to serial line
  //serial(PSTR("%d;%d;%d;%d;%d;%s\n"), message.sender, message.sensor, mGetCommand(message), mGetAck(message), message.type, message.getString(convBuf));

  if (message.type == V_ARMED) {
    Serial.print("Boulder ");
    Serial.print(message.sensor);
    Serial.print(": ");
    Serial.println("Armed");
  }

  if (message.type == V_TRIPPED) {
    if (message.getBool()) {
      Serial.print("Boulder ");
      Serial.print(message.sensor);
      Serial.println(": Motion Detected");

      if (message.sensor == 1) {
        MyMessage msg1(message.sensor, V_VAR1);
        gw.send(msg1.set(0));

        MyMessage msg2(2, V_VAR1);
        gw.send(msg2.set(0));
      }

      if (message.sensor == 2) {
        MyMessage msg1(message.sensor, V_VAR1);
        gw.send(msg1.set(1));

        MyMessage msg2(1, V_VAR1);
        gw.send(msg2.set(1));
      }

    } else {
      Serial.print("Boulder ");
      Serial.print(message.sensor);
      Serial.println(": Motion Ended");

      if (message.sensor == 1) {
        MyMessage msg1(message.sensor, V_VAR1);
        gw.send(msg1.set(2));

        MyMessage msg2(2, V_VAR1);
        gw.send(msg2.set(2));
      }

      if (message.sensor == 2) {
        MyMessage msg1(message.sensor, V_VAR1);
        gw.send(msg1.set(2));

        MyMessage msg2(1, V_VAR1);
        gw.send(msg2.set(2));
      }
    }
  }
}




