//#define USE_SERIAL_CMDS

#include <FastLED.h>
#include <MotionSensor.h>
#include <MySensor.h>
#include <SPI.h>
#include "config.h"
#include "power_mgt.h"
#include "Cell.h"
#include "ColorWipe.h"

#ifdef USE_SERIAL_CMDS
#include <SerialCommand.h>
#endif

typedef enum PaintMode {
  ModeDoNothing = 0,                // this will basically freeze the cells in their current state
  ModeAlternateDriftSinusoidal,
  ModeColorWipe,
  ModeFillGradient,
  ModeLightning,
  ModeMover,
  ModeRainbowBeat
} PaintMode;


//
// Variables
//

CRGB          leds[NUM_LEDS];
Cell          cells[NUM_CELLS];
MotionSensor  sensors[NUM_SENSORS];     // motion sensors
MySensor      gw;                       // radio
PaintMode     paintMode = ModeColorWipe;
ColorWipe     colorWipe(leds);

#ifdef USE_SERIAL_CMDS
SerialCommand sCmd;                     // command processor
#endif

//
// Functions
//

void(* resetFunc) (void) = 0;


void setup() {
  Serial.begin(115200);

  // setupStorage();   // always do this first
  setupFastLED();
  setupCells();

  colorWipe.setColor(CHSV(random8(), 208, UINT8_MAX));
  colorWipe.setInterval(250);


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
  case ModeAlternateDriftSinusoidal:
    ManualUpdate();
    break;

  case ModeColorWipe:
    colorWipe.update();
    break;

  case ModeRainbowBeat:
    RainbowBeat();
    break;

  case ModeMover:
    Mover();
    break;

  case ModeLightning:
    Lightning();
    break;

  case ModeFillGradient:
    FillGradient();
    break;

  case ModeDoNothing:
  default:
    // noop
    break;
  }

  if (0 == colorWipe.getCurrentPixelIndex()) {
    colorWipe.setColor(CHSV(random8(), 224, UINT8_MAX));
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


// Routines

void ManualUpdate() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < NUM_CELLS; i++) {
    cells[i].update(now);
  }
}

void RainbowBeat() {
  uint8_t beatA = beatsin8(5, 0, 255);                        // Starting hue
  uint8_t beatB = beatsin8(15, 4, 20);                        // Delta hue between LED's
  fill_rainbow(leds, NUM_LEDS, beatA, beatB);                 // Use FastLED's fill_rainbow routine.
}


// TODO: parameterize the fade
#define MOVER_FADE 64                 // Low values = slower fade
void Mover(){
  static uint16_t i = 0;
  static uint8_t hue = 0;

  EVERY_N_MILLISECONDS(100) {                                         // UGH!!!! A blocking delay. If you want to add controls, they may not work reliably.
    leds[i] += CHSV(hue, 255, 255);
    leds[(i+5) % NUM_LEDS] += CHSV(hue+85, 255, 255);         // We use modulus so that the location is between 0 and NUM_LEDS
    leds[(i+10) % NUM_LEDS] += CHSV(hue+170, 255, 255);       // Same here.
    show_at_max_brightness_for_power();
    fadeToBlackBy(leds, NUM_LEDS, MOVER_FADE);
    i++;
    i %= NUM_LEDS;
  }

}


#define FREQUENCY 50                                       // controls the interval between strikes
#define FLASHES   8
void Lightning(){
  static unsigned int dimmer = 1;
  static uint8_t ledstart;                                             // Starting location of a flash
  static uint8_t ledlen;                                               // Length of a flash

  EVERY_N_MILLISECONDS(random8(FREQUENCY)*100){          // delay between strikes
    ledstart = random8(NUM_LEDS);           // Determine starting location of flash
    ledlen = random8(NUM_LEDS-ledstart);    // Determine length of flash (not to go beyond NUM_LEDS-1)
    for (int flashCounter = 0; flashCounter < random8(3,FLASHES); flashCounter++) {
      EVERY_N_MILLISECONDS(50+random8(100)) {
        if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
        else dimmer = random8(1,3);           // return strokes are brighter than the leader
        fill_solid(leds+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
        FastLED.show();                       // Show a section of LED's
        delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
        fill_solid(leds+ledstart,ledlen,CHSV(255,0,0));   // Clear the section of LED's
        FastLED.show();
        if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
      }
    } // for()
  }
}

void FillGradient(){
  uint8_t starthue = beatsin8(20, 0, 255);
  uint8_t endhue = beatsin8(35, 0, 255);
  if (starthue < endhue) {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);    // If we don't have this, the colour fill will flip around
  } else {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), BACKWARD_HUES);
  }
}

void setBlack() {
  for (int i = 0; i < NUM_CELLS; i++) {
    leds[i] = CRGB::Black;
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
    Serial.println("ModeDoNothing");
    break;

  case ModeAlternateDriftSinusoidal:
    paintMode = ModeAlternateDriftSinusoidal;
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
      cells[i].setHue(random8());
      cells[i].setHueMode(Cell::ModeAlternateDrift);
      cells[i].setHueInterval(500U);
      cells[i].setHueIntervalMode(Cell::ModeSinusoidal);
      cells[i].setSaturationMode(Cell::ModeConstant);
      cells[i].setSaturationIntervalMode(Cell::ModeConstant);
      cells[i].setValueMode(Cell::ModeConstant);
      cells[i].setValueIntervalMode(Cell::ModeConstant);
    }
    Serial.println("ModeAlternateDriftSinusoidal");
    break;

  case ModeColorWipe:
    paintMode = ModeColorWipe;
    Serial.println("ModeColorWipe");
    break;

  case ModeRainbowBeat:
    paintMode = ModeRainbowBeat;
    Serial.println("ModeRainbowBeat");
    break;

  case ModeMover:
    paintMode = ModeMover;
    Serial.println("ModeMover");
    break;

  case ModeLightning:
    setBlack();
    paintMode = ModeLightning;
    Serial.println("ModeLightning");
    break;

  case ModeFillGradient:
    paintMode = ModeFillGradient;
    Serial.println("ModeFillGradient");
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
