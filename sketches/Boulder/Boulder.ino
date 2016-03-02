#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <SerialCommand.h>
#include "config.h"
#include "Nodule.h"

// the address in EEPROM where we store the nodule count
#define NUM_NODULES_ADDR 0

//
// Variables
//

uint16_t      numLEDs;
uint8_t       numNodules;
CRGB          *leds;
Nodule        *nodules;
SerialCommand sCmd;


//
// Functions
//

void(* resetFunc) (void) = 0;


void setup() {
  Serial.begin(115200);

  setupStorage();   // always do this first
  setupFastLED();
  setupNodules();
  setupSerialCommands();

  Serial.print(numNodules);
  Serial.println(" nodules started");
}


void loop() {
  uint32_t now = millis();

  for (uint8_t i = 0; i < numNodules; i++) {
    nodules[i].update(now);
  }

  FastLED.show();
  sCmd.readSerial();
}


void setupFastLED() {
  FastLED.addLeds<WS2812B, 6, RGB>(leds, numLEDs).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}


void setupNodules() {
  for (uint8_t i = 0; i < numNodules; i++) {
    CRGB *nodeStart = &(leds[i * LEDS_PER_NODULE]);
    nodules[i] = Nodule(nodeStart);
    // nodules[i].setHueInterval(i*7+20);
    // nodules[i].setValueMode(Nodule::ModeSinusoidal);
    // nodules[i].setValueInterval(i*33);
  }
}


void setupSerialCommands() {
  sCmd.addCommand("nodules", setNoduleCount);
  sCmd.addCommand("reset", resetFunc);
  sCmd.addCommand("set", setNodule);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched
}


void setupStorage() {
  // Wait for EEPROM to be ready
  while (!eeprom_is_ready());
  cli();
  numNodules = EEPROM.read(NUM_NODULES_ADDR);
  sei();
  nodules    = new Nodule[numNodules];
  numLEDs    = numNodules * LEDS_PER_NODULE;
  leds       = new CRGB[numLEDs];
}

void setNodule() {

}

void setNoduleCount() {
  int noduleCount;
  char *arg = sCmd.next();

  if (arg == NULL) {
    Serial.println("No arguments to 'nodules'");
    return;
  }

  noduleCount = atoi(arg);
  if (noduleCount < 0 || noduleCount > UINT8_MAX) {
    Serial.println("Got a weird number for 'nodules'");
    return;
  }

  while (!eeprom_is_ready());
  cli();
  EEPROM.update(NUM_NODULES_ADDR, noduleCount);
  sei();
  Serial.print("nodules: ");
  Serial.println(noduleCount);
  Serial.println("don't forget to 'reset'...");
}


// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
