#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <SerialCommand.h>
#include "config.h"
#include "Cell.h"

// the address in EEPROM where we store the cell count
#define NUM_CELLS_ADDR 0

//
// Variables
//

uint16_t      numLEDs;
uint8_t       numCells;
CRGB          *leds;
Cell        *cells;
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
  Serial.println(" cells started");
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
  FastLED.addLeds<WS2812B, 6, RGB>(leds, numLEDs).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}


void setupCells() {
  for (uint8_t i = 0; i < numCells; i++) {
    CRGB *nodeStart = &(leds[i * LEDS_PER_CELL]);
    cells[i] = Cell(nodeStart);
    // cells[i].setHueInterval(i*7+20);
    // cells[i].setValueMode(Cell::ModeSinusoidal);
    // cells[i].setValueInterval(i*33);
  }
}


void setupSerialCommands() {
  sCmd.addCommand("cells", setCellCount);
  sCmd.addCommand("reset", resetFunc);
  sCmd.addCommand("set", setCell);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched
}


void setupStorage() {
  // Wait for EEPROM to be ready
  while (!eeprom_is_ready());
  cli();
  numCells = EEPROM.read(NUM_CELLS_ADDR);
  sei();
  cells    = new Cell[numCells];
  numLEDs    = numCells * LEDS_PER_CELL;
  leds       = new CRGB[numLEDs];
}

void setCell() {

}

void setCellCount() {
  int cellCount;
  char *arg = sCmd.next();

  if (arg == NULL) {
    Serial.println("No arguments to 'cells'");
    return;
  }

  cellCount = atoi(arg);
  if (cellCount < 0 || cellCount > UINT8_MAX) {
    Serial.println("Got a weird number for 'cells'");
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


// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
