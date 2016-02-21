



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

    MotionSensor(int id, int pin, long unsigned calibTime);

    void setPauseTime(long unsigned int ms);

    void startSensor();
    void updateSensor();

    //Sensor Callback Functions
    int(*fpMotionSensorAction)(int, int) = 0;
    void setMotionSensorDelegate(int(*fp)(int, int));
    int motionSensorEvent(int sensorId, int e);

    int mId;
    int mPin;

  private:

    long unsigned int mLowIn;
    long unsigned int mPauseTime;
    long unsigned int mCalibTime;

    boolean mLockLow;
    boolean mTakeLowTime;
    boolean mCalibrated;
};

#endif
