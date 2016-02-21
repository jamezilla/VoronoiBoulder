
#include "MotionSensor.h"

int ledPin = 13;

MotionSensor mSensor = MotionSensor(0,9, 30000);

void setup() {

  mSensor1.setPauseTime(5000);
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  mSensor.setMotionSensorDelegate(onMotionSensorEvent);
}

void loop()
{
  if(mSensor1.isCalibrated()) {
    mSensor.updateSensor();
    digitalWrite(ledPin,LOW);
  } else {
    mSensor.calibrateSensor();
    digitalWrite(ledPin,HIGH);
  }
}

int onMotionSensorEvent(int id,int e) {
  if(e==MotionSensor::SENSOR_MOTION_CALIBRATING) {
    mSensor.calibrateSensor();  
  }
  if(e==MotionSensor::SENSOR_MOTION_DETECTED) {
    Serial.print("Sensor ");
    Serial.print(id);
    Serial.println(" Event: Motion Detected");
    
  }
  if(e==MotionSensor::SENSOR_MOTION_ENDED) {
    Serial.print("Sensor ");
    Serial.print(id);
    Serial.println(" Event: Motion Ended");
  }
}






