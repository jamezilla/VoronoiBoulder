#ifndef MotionSensor_h
#define MotionSensor_h

#include "Arduino.h"

class MotionSensor
{
  public:

    enum sensor_status : byte
    {
      SENSOR_MOTION_CALIBRATING,
      SENSOR_MOTION_ACTIVE,
      SENSOR_MOTION_DETECTED,
      SENSOR_MOTION_ENDED
    };

    struct sensorEventArgs {
      int id;
      sensor_status state;
      long unsigned int timeStamp;
    };

    MotionSensor();
    MotionSensor(int id, int pin, long unsigned calibTime);

    void setPauseTime(long unsigned int ms);

    void updateSensor();

    //Sensor Callback Functions
    int(*fpMotionSensorAction)(sensorEventArgs) = 0;
    void setMotionSensorDelegate(int(*fp)(sensorEventArgs));
    int motionSensorEvent(sensorEventArgs e);

    int mId;
    int mPin;

    sensorEventArgs mArgs;

  private:

    long unsigned int mLowIn;
    long unsigned int mPauseTime;
    long unsigned int mCalibTime;

    boolean mLockLow;
    boolean mTakeLowTime;
    boolean mCalibrated;
};

#endif
