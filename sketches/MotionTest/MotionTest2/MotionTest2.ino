#include "MotionSensor.h"

int ledPin = 13;

MotionSensor mSensor = MotionSensor(0,9,30000);

void setup() {

  mSensor.setPauseTime(5000);
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  mSensor.setMotionSensorDelegate(onMotionSensorEvent);
}

void loop()
{
  mSensor.updateSensor();
}

int onMotionSensorEvent(int id,int e) {
  
  if(e==MotionSensor::SENSOR_MOTION_CALIBRATING) {
    digitalWrite(ledPin,HIGH);
  }

  if(e==MotionSensor::SENSOR_MOTION_ACTIVE) {
    
    digitalWrite(ledPin,LOW);
    
    Serial.print("Sensor ");
    Serial.print(id);
    Serial.println(" Event: Sensor Active");
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






