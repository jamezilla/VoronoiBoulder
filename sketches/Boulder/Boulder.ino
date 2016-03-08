#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <SerialCommand.h>
#include "config.h"
#include "Cell.h"

// the address in EEPROM where we store the cell count
#define NUM_CELLS_ADDR 0
#define NUM_LEDS_ADDR  1

//
// Variables
//

uint16_t      numLeds;
uint8_t       numCells;
CRGB          *leds;
Cell          *cells;
SerialCommand sCmd;


//
// Functions
//

void(* resetFunc) (void) = 0;


void setup() {
  Serial.begin(115200);

  setupStorage();   // always do this first
  setupFastLED();
  setupCells();
  setupSerialCommands();

  Serial.print(numCells);
  Serial.print(" cells ");
  Serial.print(numLeds);
  Serial.print(" leds");
}


void loop() {
  uint32_t now = millis();

  for (uint8_t i = 0; i < numCells; i++) {
    cells[i].update(now);
  }

  FastLED.show();
  sCmd.readSerial();
}


void setupFastLED() {
  FastLED.addLeds<WS2812B, 6, RGB>(leds, numLeds).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}


void setupCells() {
  uint8_t sliceSize = numLeds / numCells;

  for (uint8_t i = 0; i < numCells; i++) {
    CRGB *nodeStart = &(leds[i * sliceSize]);
    cells[i] = Cell(nodeStart);
    // cells[i].setHueInterval(i*7+20);
    // cells[i].setValueMode(Cell::ModeSinusoidal);
    // cells[i].setValueInterval(i*33);
  }
}


void setupSerialCommands() {
  sCmd.addCommand("c", setCellCount);
  sCmd.addCommand("l", setLedCount);
  sCmd.addCommand("r", resetFunc);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched
}


void setupStorage() {
  uint8_t cellCount;
  uint16_t ledCount;

  // Wait for EEPROM to be ready
  while (!eeprom_is_ready());
  cli();
  cellCount = EEPROM.read(NUM_CELLS_ADDR);
  ledCount  = EEPROM.read(NUM_LEDS_ADDR);
  sei();

  if (cellCount == NULL || cellCount == 0) {
    numCells = 1;
  } else {
    numCells = cellCount;
  }

  if (ledCount == NULL || ledCount == 0) {
    numLeds = numCells;
  } else {
    numLeds = ledCount;
  }

  cells = new Cell[numCells];
  leds  = new CRGB[numLeds];
}

void setCellCount() {
  int cellCount;
  char *arg = sCmd.next();

  if (arg == NULL) {
    Serial.println("No arguments to 'c'");
    return;
  }

  cellCount = atoi(arg);
  if (cellCount < 0 || cellCount > UINT8_MAX) {
    Serial.println("Got a weird number for 'c'");
    return;
  }

  while (!eeprom_is_ready());
  cli();
  EEPROM.update(NUM_CELLS_ADDR, cellCount);
  sei();
  Serial.print("cells: ");
  Serial.println(cellCount);
  Serial.println("don't forget to 'reset'...");
}

void setLedCount() {
  int ledCount;
  char *arg = sCmd.next();

  if (arg == NULL) {
    Serial.println("No arguments to 'l'");
    return;
  }

  ledCount = atoi(arg);
  if (ledCount < 0 || ledCount > UINT8_MAX) {
    Serial.println("Got a weird number for 'l'");
    return;
  }

  while (!eeprom_is_ready());
  cli();
  EEPROM.update(NUM_LEDS_ADDR, ledCount);
  sei();
  Serial.print("leds: ");
  Serial.println(ledCount);
  Serial.println("don't forget to 'reset'...");
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}