#define USE_SERIAL_CMDS

#include <FastLED.h>
#include <MotionSensor.h>
#include <MySensor.h>
#include <SPI.h>
#include "config.h"
#include "power_mgt.h"
#include "Cell.h"
#include "ColorWhipe.h"

#ifdef USE_SERIAL_CMDS
#include <SerialCommand.h>
#endif

typedef enum PaintMode {
  ModeDoNothing = 0,
  ModeManual,
  ModeColorWhipe,
} PaintMode;


//
// Variables
//

CRGB          leds[NUM_LEDS];
Cell          cells[NUM_CELLS];
MotionSensor  sensors[NUM_SENSORS];     // motion sensors
MySensor      gw;                       // radio
SerialCommand sCmd;                     // command processor

PaintMode     paintMode = ModeColorWhipe;
ColorWhipe    colorWhipe(leds);

//
// Functions
//

void(* resetFunc) (void) = 0;


void setup() {
  Serial.begin(115200);

  // setupStorage();   // always do this first
  setupFastLED();
  setupCells();

  colorWhipe.setColor(CHSV(random8(), 208, UINT8_MAX));
  colorWhipe.setInterval(250);


  //Config Radio Communication
  gw.begin(onIncomingMessageEvent,BOULDER_ID);
  gw.sendSketchInfo("BoulderFirmware", "0.1");

  setupSensors();

#ifdef USE_SERIAL_CMDS
  setupSerialCommands();
  Serial.print(NUM_CELLS);
  Serial.print(" cells ");
  Serial.print(NUM_LEDS);
  Serial.print(" leds");
#endif
}


void loop() {
  uint32_t now = millis();

  // update sensors
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensors[i].updateSensor();
  }

  // update radio
  gw.process();

  // update leds
  switch(paintMode) {
  case ModeColorWhipe:
    colorWhipe.update();
    break;
  case ModeManual:
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
      cells[i].update(now);
    }
    break;
  case ModeDoNothing:
  default:
    // noop
    break;
  }

  if (0 == colorWhipe.getCurrentPixelIndex()) {
    colorWhipe.setColor(CHSV(random8(), 208, UINT8_MAX));
  }

#ifdef PWRMGT
  show_at_max_brightness_for_power();
#else
  FastLED.show();
#endif

#ifdef USE_SERIAL_CMDS
  // process any serial commands
  sCmd.readSerial();
#endif
}


void setupFastLED() {
#ifdef CHIP_APA102
  FastLED.addLeds<APA102, 6, 7>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
#else
  FastLED.addLeds<WS2812B, 6, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
#endif

#ifdef PWRMGT
  set_max_power_in_volts_and_milliamps(VOLTAGE, MAXCURRENT);
#else
  FastLED.setBrightness(BRIGHTNESS);
#endif
}


void setupCells() {
  uint8_t sliceSize = NUM_LEDS / NUM_CELLS;
  Serial.print("sliceSize: ");
  Serial.println(sliceSize);

  for (uint8_t i = 0; i < NUM_CELLS; i++) {
    CRGB *nodeStart = leds + (i * sliceSize);
    cells[i] = Cell(nodeStart, sliceSize);
  }
}


void setupSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensors[i] = MotionSensor(i, i+2, 30000);
    sensors[i].setPauseTime(PAUSETIME);
    sensors[i].setMotionSensorDelegate(onMotionSensorEvent);
    gw.present(i, S_MOTION);
  }
}


//Radio Event/////////////////////////////////////////////////
void onIncomingMessageEvent(const MyMessage &message) {

  //Change Boulder RGB Color
  if (message.type == V_RGB) {
    // mBoulder.setBoulderColor(message.getString());
  }

}

void calibrating() {
  cells[0].setHue(255);
  cells[1].setHue(64);
  cells[2].setHue(128);
}

void active() {
  for (uint8_t i = 0; i < NUM_CELLS; i++) {
    cells[i].setHue(64);
  }
}

void detected() {
  cells[0].setHue(0);
  cells[1].setHue(64);
  cells[2].setHue(128);
}

void ended() {
  for (uint8_t i = 0; i < NUM_CELLS; i++) {
    cells[i].setHue(192);
  }
}

// acknowledge first trigger
// accumulate triggers until you reach a threshold, then explosion/glitter, then back to a steady state

// bubbling up - slow fade from bottom
// each trigger: snake up

// random fade in each cell

// ripple

//Motion Sensor Event/////////////////////////////////////////
int onMotionSensorEvent(const MotionSensor::sensorEventArgs e) {

  int id = e.id;
  int state = e.state;
  unsigned long timeStamp = e.timeStamp;

  // Serial.print("Sensor ");
  // Serial.print(id);

  switch (state) {
  //Sensor Calibrating Event
  case MotionSensor::SENSOR_MOTION_CALIBRATING:
    {
      // Serial.println(" calibrating");
      // calibrating();
      break;
    }
    // Sensor Armed Event
  case MotionSensor::SENSOR_MOTION_ACTIVE:
    {
      // Serial.println(" active");
      // active();

      MyMessage sensorArmedMsg(id, V_ARMED);
      gw.send(sensorArmedMsg.set(1));
      break;
    }
    //Sensor Motion Detected Event
  case MotionSensor::SENSOR_MOTION_DETECTED:
    {
      // Serial.println(" detected");
      // detected();

      MyMessage sensorTrippedMsg(id, V_TRIPPED);
      gw.send(sensorTrippedMsg.set(1));
      break;
    }
    //Sensor Motion Ended Event
  case MotionSensor::SENSOR_MOTION_ENDED:
    {
      // Serial.println(" ended");
      // ended();

      MyMessage sensorTrippedMsg(id, V_TRIPPED);
      gw.send(sensorTrippedMsg.set(0));
      break;
    }
  default:
    {
      // Serial.print(" unknown ");
      // Serial.println(e.state);
    }
  }

  return 0;
}


#ifdef USE_SERIAL_CMDS

void setPaintMode() {
  int mode;
  char *arg = sCmd.next();

  if (arg == NULL) {
    Serial.println("No arguments to 'm'");
    return;
  }

  switch(atoi(arg)) {
  case ModeDoNothing:
    paintMode = ModeDoNothing;
    break;
  case ModeColorWhipe:
    paintMode = ModeColorWhipe;
    break;
  default:
    Serial.print("illegal mode: ");
    Serial.println(arg);
  }
}


// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}


void setupSerialCommands() {
  sCmd.addCommand("m", setPaintMode);
  sCmd.addCommand("r", resetFunc);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched
}

#endif
