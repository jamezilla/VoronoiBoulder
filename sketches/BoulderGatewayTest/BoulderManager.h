#include "Arduino.h"

#define NUMBOULDERS 6
#define NUMSENSORS 4

struct SensorData {
  bool armed = false;
  bool tripped = false;
};

struct BoulderData {
  SensorData sensorData[NUMSENSORS];
  String color = "000000";
};

class BoulderManager {

  public:

    BoulderManager();

    //Boulder Data Functions
    void setBoulderSensorArmed(int bId,int sId, bool _armed);
    void setBoulderSensorTripped(int bId,int sId,bool _tripped);

    int getNumArmedSensors(int bId);
    int getNumTrippedSensors(int bId);
    String getBoulderHexColor(int bId);
    
    //Boulder Color Functions
    void setBoulderColorActivity(int bId);
    
    BoulderData mBoulderData[NUMBOULDERS];
};

