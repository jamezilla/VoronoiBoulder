#include "Arduino.h"
#include "MotionSensor.h"

unsigned long int calibTimer = 0;
unsigned long int motionTimer = 0;

MotionSensor::MotionSensor()
{

}

MotionSensor::MotionSensor(int id, int pin, unsigned long calibTime)
{
  mPin = pin;
  mId = id;
  mLockLow = true;
  mCalibTime = calibTime;
  mCalibrated = false;
  calibTimer = millis();

  mArgs.id = mId;

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

        mArgs.state = SENSOR_MOTION_DETECTED;
        mArgs.timeStamp = millis();
        //Trigger Sensor Motion Detected Event
        motionSensorEvent(mArgs);
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

        //Calculate Elapsed Time from Prev State (SENSOR_MOTION_DETECTED)
        mArgs.state = SENSOR_MOTION_ENDED;
        mArgs.timeStamp = (millis() - mPauseTime) - mArgs.timeStamp;

        //Trigger SENSOR_MOTION_ENDED Event
        motionSensorEvent(mArgs);
      }
    }
  } else {
    if ((!mCalibrated) && ((millis() - calibTimer) > mCalibTime)) {

      mCalibrated = true;

      mArgs.state = SENSOR_MOTION_ACTIVE;
      mArgs.timeStamp = millis();

      //Trigger SENSOR_MOTION_ACTIVE Event
      motionSensorEvent(mArgs);
    } else {

      mArgs.state = SENSOR_MOTION_CALIBRATING;
      mArgs.timeStamp = millis();

      //Trigger SENSOR_MOTION_CALIBRATING Event
      motionSensorEvent(mArgs);
    }
  }
}

void MotionSensor::setMotionSensorDelegate(int(*fp)(sensorEventArgs)) {
  fpMotionSensorAction = fp;
}

int MotionSensor::motionSensorEvent(sensorEventArgs e) {
  int r;
  if (0 != fpMotionSensorAction) {
    r = (*fpMotionSensorAction)(e);
  } else {
    r = 0;
  }
  return r;
}





