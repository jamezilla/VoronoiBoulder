#include "Arduino.h"
#include "MotionSensor.h"

unsigned long int calibTimer = 0;
unsigned long int motionTimer = 0;

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
  sensorEventArgs args;
  args.id = mId;
  
  if (mCalibrated) {
    if (digitalRead(mPin) == HIGH) {

      if (mLockLow) {
        //makes sure we wait for a transition to LOW before any further output is made:
        mLockLow = false;

        args.state = SENSOR_MOTION_DETECTED;
        args.timeStamp = millis();
        
        motionSensorEvent(args);
      }
      mTakeLowTime = true;

      args.state = SENSOR_MOTION_ACTIVITY;
      args.timeStamp = millis()-calibTimer;
      
      motionSensorEvent(args);
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

        args.state = SENSOR_MOTION_ENDED;
        args.timeStamp = millis();

        motionSensorEvent(args);
      }
    }
  } else {
    if ((!mCalibrated) && ((millis() - calibTimer) > mCalibTime)) {

      mCalibrated = true;

      args.state = SENSOR_MOTION_ACTIVE;
      args.timeStamp = millis();

      motionSensorEvent(args);
    } else {

      args.state = SENSOR_MOTION_CALIBRATING;
      args.timeStamp = millis();
      
      motionSensorEvent(args);
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





