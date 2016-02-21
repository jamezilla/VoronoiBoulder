#include <MySensor.h>
#include <SPI.h>

#include "MotionSensor.h"

#define CHILD_ID 2   // Id of the sensor child

//Config Sensors
MotionSensor mSensor = MotionSensor(0, 2, 30000);

MySensor gw;
MyMessage msg(CHILD_ID, V_TRIPPED);

int onMotionSensorEvent(MotionSensor::sensorEventArgs e) {

  int id = e.id;
  int state = e.state;
  unsigned long timeStamp = e.timeStamp;

  if (state == MotionSensor::SENSOR_MOTION_CALIBRATING) {
    //sendMessage(state);
  }
  if (state == MotionSensor::SENSOR_MOTION_ACTIVE) {
    Serial.print("Sensor ");
    Serial.print(e.id);
    Serial.print(": Active");

    sendMessage(state);
  }
  if (state == MotionSensor::SENSOR_MOTION_DETECTED) {
    Serial.print("Sensor ");
    Serial.print(e.id);
    Serial.print(": Motion Detected");

    sendMessage(state);
  }
  if (state == MotionSensor::SENSOR_MOTION_ACTIVITY) {

  }
  if (state == MotionSensor::SENSOR_MOTION_ENDED) {
    Serial.print("Sensor ");
    Serial.print(e.id);
    Serial.print(": Motion Ended");

    sendMessage(state);
  }
}

void setup() {
  //Config Motion Sensor
  mSensor.setPauseTime(1000);
  mSensor.setMotionSensorDelegate(onMotionSensorEvent);

  gw.begin();

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("MotionRFTest","1.0");
  gw.present(CHILD_ID, S_MOTION);

  Serial.begin(115200);
}

void loop()
{
  mSensor.updateSensor();
}

void sendMessage(int data) {
  gw.send(msg.set(data));  // Send tripped value to gw
}










