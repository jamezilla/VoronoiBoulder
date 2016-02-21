#include <MotionSensor.h>
#include <MySensor.h>
#include <SPI.h>
#include <FastLED.h>
#include "power_mgt.h"
#include "Boulder.h"

#define PWRMGT
#define VOLTAGE 5
#define MAXCURRENT 100

//#define CHIP_WS281B
#define CHIP_APA102

#define BOULDER_ID 1
#define NUMSENSORS 4
#define PAUSETIME  1000
#define NUMLEDS    10
#define BRIGHTNESS 75

//Boulder Vars
Boulder mBoulder;

//Sensor Vars
MotionSensor mSensors[NUMSENSORS];

//Radio Vars
MySensor  mGw;

//Lighting Vars
CRGB mLeds[NUMLEDS];

//Radio Event/////////////////////////////////////////////////
void onIncomingMessageEvent(const MyMessage &message) {

  //Change Boulder RGB Color
  if (message.type == V_RGB) {
    mBoulder.setBoulderColor(message.getString());
  }

}

//Motion Sensor Event/////////////////////////////////////////
int onMotionSensorEvent(const MotionSensor::sensorEventArgs e) {

  int id = e.id;
  int state = e.state;
  unsigned long timeStamp = e.timeStamp;

  //Sensor Calibrating Event
  if (state == MotionSensor::SENSOR_MOTION_CALIBRATING) {

  }

  //Sensor Armed Event
  if (state == MotionSensor::SENSOR_MOTION_ACTIVE) {
    Serial.print("Sensor ");
    Serial.print(id);
    Serial.println(" : Active");

    MyMessage sensorArmedMsg(id, V_ARMED);
    mGw.send(sensorArmedMsg.set(1));
  }

  //Sensor Motion Detected Event
  if (state == MotionSensor::SENSOR_MOTION_DETECTED) {
    Serial.print("Sensor ");
    Serial.print(id);
    Serial.println(" : Motion Detected");

    MyMessage sensorTrippedMsg(id, V_TRIPPED);
    mGw.send(sensorTrippedMsg.set(1));
  }

  //Sensor Motion Ended Event
  if (state == MotionSensor::SENSOR_MOTION_ENDED) {
    Serial.print("Sensor ");
    Serial.print(id);
    Serial.println(" : Motion Ended");

    MyMessage sensorTrippedMsg(id, V_TRIPPED);
    mGw.send(sensorTrippedMsg.set(0));
  }
}

void setup() {

  Serial.begin(115200);

  //Config Sensors
  mSensors[0] = MotionSensor(0, 2, 30000);
  mSensors[1] = MotionSensor(1, 3, 30000);
  mSensors[2] = MotionSensor(2, 4, 30000);
  mSensors[3] = MotionSensor(3, 5, 30000);

  for (int i = 0; i < NUMSENSORS; i++) {
    mSensors[i].setPauseTime(PAUSETIME);
    mSensors[i].setMotionSensorDelegate(onMotionSensorEvent);
  }

  //Config Radio Communication
  mGw.begin(onIncomingMessageEvent,BOULDER_ID);
  mGw.sendSketchInfo("BoulderFirmware", "0.1");

  for(int i=0;i<NUMSENSORS;i++) {
    mGw.present(i, S_MOTION);
  }

  //Config Lighting
#ifdef CHIP_WS281B
  FastLED.addLeds<WS2812B, 6, GRB>(mLeds, NUMLEDS).setCorrection( TypicalLEDStrip );
#endif

#ifdef CHIP_APA102
  FastLED.addLeds<APA102, 6, 7>(mLeds, NUMLEDS).setCorrection(TypicalLEDStrip);
#endif


#ifdef PWRMGT
  set_max_power_in_volts_and_milliamps(VOLTAGE,MAXCURRENT);
#else
  FastLED.setBrightness(BRIGHTNESS);
#endif

  for (int i = 0; i < NUMLEDS; i++) {
    mLeds[i] = CRGB::Black;
  }

#ifdef PWRMGT
  show_at_max_brightness_for_power();
#else
  FastLED.show();
#endif
}

void loop() {

  //Update Sensors
  for (int i = 0; i < NUMSENSORS; i++) {
    mSensors[i].updateSensor();
  }

  //Update Radio
  mGw.process();

  //Update Lighting
  for (int i = 0; i < NUMLEDS; i++) {

    rgb bColor = mBoulder.getBoulderColor();

    mLeds[i] = CRGB(bColor.r, bColor.g, bColor.b);
  }

#ifdef PWRMGT
  show_at_max_brightness_for_power();
#else
  FastLED.show();
#endif
}







