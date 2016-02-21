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
    
    MotionSensor(int id,int pin, long unsigned calibTime);

    void setPauseTime(long unsigned int ms);

    void calibrateSensor();
    bool isCalibrated();

    int(*fpMotionSensorAction)(int,int) = 0;
    void setMotionSensorDelegate(int(*fp)(int,int));
    int motionSensorEvent(int sensorId,int e);

    void  updateSensor();

    int mId;
    int mPin;

    long unsigned int mLowIn;
    long unsigned int mPauseTime;
    long unsigned int mCalibTime;

  private:

    boolean mLockLow;
    boolean mTakeLowTime;
    boolean mCalibrated;

};

#endif
