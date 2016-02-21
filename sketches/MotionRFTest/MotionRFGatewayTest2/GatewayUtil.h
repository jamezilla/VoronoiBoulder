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
 */

#ifndef __GATEWAYUTIL_H__
#define __GATEWAYUTIL_H__

#include <MyTransport.h>
#include "BoulderManager.h"

#ifdef ARDUINO

#define MAX_RECEIVE_LENGTH 100 // Max buffersize needed for messages coming from controller
#define MAX_SEND_LENGTH 120 // Max buffersize needed for messages destined for controller

void (*serial)(const char *fmt, ... );

MyParserSerial parser;

char convBuf[MAX_PAYLOAD * 2 + 1];
char serialBuffer[MAX_SEND_LENGTH]; // Buffer for building string when sending data to vera

void setupGateway(void (* _serial)(const char *, ... )) {
  serial = _serial;
}




void parseAndSend(MySensor &gw, char *commandBuffer) {
  boolean ok;
  MyMessage &msg = gw.getLastMessage();

  if (parser.parse(msg, commandBuffer)) {
    uint8_t command = mGetCommand(msg);

    if (msg.destination == GATEWAY_ADDRESS && command == C_INTERNAL) {
      // Handle messages directed to gateway
      if (msg.type == I_VERSION) {
        // Request for version
        //serial(PSTR("0;0;%d;0;%d;%s\n"), C_INTERNAL, I_VERSION, LIBRARY_VERSION);
      }
    }
  }
}


#else
#error This example is only for use on Arduino.
#endif // ARDUINO

#endif

