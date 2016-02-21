#include "Arduino.h"
#include "MotionSensor.h"

unsigned long int calibTimer = 0;

MotionSensor::MotionSensor(int id, int pin, unsigned long calibTime)
{
  mPin = pin;
  mId = id;
  mLockLow = true;
  mCalibTime = calibTime;
  mCalibrated = false;
  calibTimer = millis();
  
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);
}

void MotionSensor::setPauseTime(unsigned long int ms)
{
  mPauseTime = ms;
}

void MotionSensor::updateSensor()
{
  if (mCalibrated) {
    if (digitalRead(mPin) == HIGH) {

      if (mLockLow) {
        //makes sure we wait for a transition to LOW before any further output is made:
        mLockLow = false;

        motionSensorEvent(mId, SENSOR_MOTION_DETECTED);
      }
      mTakeLowTime = true;
    }

    if (digitalRead(mPin) == LOW) {

      if (mTakeLowTime) {
        mLowIn = millis();          //save the time of the transition from high to LOW
        mTakeLowTime = false;       //make sure this is only done at the start of a LOW phase
      }
      //if the sensor is low for more than the given pause,
      //we assume that no more motion is going to happen
      if (!mLockLow && millis() - mLowIn > mPauseTime) {
        //makes sure this block of code is only executed again after
        //a new motion sequence has been detected
        mLockLow = true;

        motionSensorEvent(mId, SENSOR_MOTION_ENDED);
      }
    }
  } else {
    if ((!mCalibrated) && ((millis() - calibTimer) > mCalibTime)) {

      mCalibrated = true;

      motionSensorEvent(mId, SENSOR_MOTION_ACTIVE);
    } else {
      motionSensorEvent(mId, SENSOR_MOTION_CALIBRATING);
    }
  }
}

void MotionSensor::setMotionSensorDelegate(int(*fp)(int, int)) {
  fpMotionSensorAction = fp;
}

int MotionSensor::motionSensorEvent(int sensorId, int e) {
  int r;
  if (0 != fpMotionSensorAction) {
    r = (*fpMotionSensorAction)(sensorId, e);
  } else {
    r = 0;
  }
  return r;
}





