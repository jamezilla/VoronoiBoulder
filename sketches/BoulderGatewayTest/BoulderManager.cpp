#include "BoulderManager.h"

String RGBToHex(int r, int g, int b)
{
  char hex[7] = {0};
  sprintf(hex, "%02X%02X%02X", r, g, b); //convert to an hexadecimal string. Lookup sprintf for what %02X means

  return String(hex);
}

BoulderManager::BoulderManager() {

}

//Boulder Data Functions/////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void BoulderManager::setBoulderSensorArmed(int bId, int sId, bool _armed) {
  mBoulderData[bId].sensorData[sId].armed = _armed;
}

void BoulderManager::setBoulderSensorTripped(int bId, int sId, bool _tripped) {
  mBoulderData[bId].sensorData[sId].tripped = _tripped;
}

int BoulderManager::getNumArmedSensors(int bId) {

  int numArmedSensors = 0;

  for(int i=0;i<NUMSENSORS;i++) {
    if(mBoulderData[bId].sensorData[i].armed) {
      numArmedSensors++;
    }
  }

  return numArmedSensors;
}

int BoulderManager::getNumTrippedSensors(int bId) {

  int numTrippedSensors = 0;

  for(int i=0;i<NUMSENSORS;i++) {
    if(mBoulderData[bId].sensorData[i].tripped) {
      numTrippedSensors++;
    }
  }

  return numTrippedSensors;
}

String BoulderManager::getBoulderHexColor(int bId) {
  return mBoulderData[bId].color;
}

//Boulder Color Functions////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void BoulderManager::setBoulderColorActivity(int bId) {

  int activeSensors = getNumTrippedSensors(bId);

  if (activeSensors == 0) {
    mBoulderData[bId].color = RGBToHex(4, 117, 111);
  }

  if (activeSensors == 1) {
    mBoulderData[bId].color = RGBToHex(255, 140, 0);
  }

  if (activeSensors == 2) {
    mBoulderData[bId].color = RGBToHex(255, 45, 0);
  }

  if (activeSensors == 3) {
    mBoulderData[bId].color = RGBToHex(217, 0, 0);
  }

  if (activeSensors == 4) {
    mBoulderData[bId].color = RGBToHex(46, 9, 39);
  }
}
