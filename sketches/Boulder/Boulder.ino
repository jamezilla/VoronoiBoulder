#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <MotionSensor.h>
#include <MySensor.h>
#include <SerialCommand.h>
#include <SPI.h>
#include "config.h"
#include "power_mgt.h"
#include "Cell.h"


#define BOULDER_ID 1
#define NUMSENSORS 1
#define PAUSETIME  1000

// the address in EEPROM where we store the cell count
#define NUM_CELLS_ADDR 0
#define NUM_LEDS_ADDR  1

//
// Variables
//

uint16_t      numLeds;                  // total # of leds on the strand
uint8_t       numCells;                 // number of cells on the whole boulder
CRGB          *leds;
Cell          *cells;
MotionSensor  sensors[NUMSENSORS];     // motion sensors
MySensor      gw;                      // radio
SerialCommand sCmd;                     // command processor
bool          ripple = false;

//
// Functions
//

void(* resetFunc) (void) = 0;


void setup() {
  Serial.begin(115200);

  setupStorage();   // always do this first
  setupFastLED();
  setupCells();

  //Config Radio Communication
  gw.begin(onIncomingMessageEvent,BOULDER_ID);
  gw.sendSketchInfo("BoulderFirmware", "0.1");

  setupSensors();
  setupSerialCommands();

  Serial.print(numCells);
  Serial.print(" cells ");
  Serial.print(numLeds);
  Serial.print(" leds");
}


void loop() {
  uint32_t now = millis();

  // update sensors
  for (int i = 0; i < NUMSENSORS; i++) {
    sensors[i].updateSensor();
  }

  // update radio
  gw.process();

  // update leds
  for (uint8_t i = 0; i < numCells; i++) {
    cells[i].update(now);
  }

#ifdef PWRMGT
  show_at_max_brightness_for_power();
#else
  FastLED.show();
#endif

  // process any serial commands
  sCmd.readSerial();
}


void setupFastLED() {
#ifdef CHIP_APA102
  FastLED.addLeds<APA102, 6, 7>(leds, numLeds).setCorrection(TypicalLEDStrip);
#else
  FastLED.addLeds<WS2812B, 6, RGB>(leds, numLeds).setCorrection(TypicalLEDStrip);
#endif

#ifdef PWRMGT
  set_max_power_in_volts_and_milliamps(VOLTAGE, MAXCURRENT);
#else
  FastLED.setBrightness(BRIGHTNESS);
#endif
}


void setupCells() {
  uint8_t sliceSize = numLeds / numCells;

  for (uint8_t i = 0; i < numCells; i++) {
    CRGB *nodeStart = &(leds[i * sliceSize]);
    cells[i] = Cell(nodeStart, sliceSize);
    cells[i].setHue(128);
    cells[i].setHueMode(Cell::ModePerlin);
    cells[i].setValueMode(Cell::ModeSawtooth);
    cells[i].setValueInterval(1);
    cells[i].setValueMin(200);
    cells[i].setValueMax(UINT8_MAX);
    // cells[i].setHueInterval(i*7+20);
    // cells[i].setValueMode(Cell::ModeSinusoidal);
    // cells[i].setValueInterval(i*33);
  }
}


void setupSensors() {
  for (int i = 0; i < NUMSENSORS; i++) {
    sensors[i] = MotionSensor(i, i+2, 30000);
    sensors[i].setPauseTime(PAUSETIME);
    sensors[i].setMotionSensorDelegate(onMotionSensorEvent);
    gw.present(i, S_MOTION);
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


//Radio Event/////////////////////////////////////////////////
void onIncomingMessageEvent(const MyMessage &message) {

  //Change Boulder RGB Color
  if (message.type == V_RGB) {
    // mBoulder.setBoulderColor(message.getString());
  }

}

//Motion Sensor Event/////////////////////////////////////////
int onMotionSensorEvent(const MotionSensor::sensorEventArgs e) {

  int id = e.id;
  int state = e.state;
  unsigned long timeStamp = e.timeStamp;

  Serial.print("Sensor ");
  Serial.print(id);

  switch (state) {
  //Sensor Calibrating Event
  case MotionSensor::SENSOR_MOTION_CALIBRATING:
    {
      Serial.println(" calibrating");
      break;
    }
    // Sensor Armed Event
  case MotionSensor::SENSOR_MOTION_ACTIVE:
    {
      Serial.println(" active");

      MyMessage sensorArmedMsg(id, V_ARMED);
      gw.send(sensorArmedMsg.set(1));
      break;
    }
    //Sensor Motion Detected Event
  case MotionSensor::SENSOR_MOTION_DETECTED:
    {
      Serial.println(" start");

      MyMessage sensorTrippedMsg(id, V_TRIPPED);
      gw.send(sensorTrippedMsg.set(1));
      break;
    }
    //Sensor Motion Ended Event
  case MotionSensor::SENSOR_MOTION_ENDED:
    {
      Serial.println(" end");

      MyMessage sensorTrippedMsg(id, V_TRIPPED);
      gw.send(sensorTrippedMsg.set(0));
      break;
    }
  default:
    {
      Serial.print(" unknown ");
      Serial.println(e.state);
    }
  }

  return 0;
}
