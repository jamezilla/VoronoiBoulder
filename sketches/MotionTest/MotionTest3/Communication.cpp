#include "Communication.h"

Communication::Communication(int bId, string firmwareName, string firmwareVer)
{
  //Init Communication
  mBoulderId = bId;
  mFirmwareName = firmwareName;
  mFirmwareVer = firmwareVer;

  //Init Messages
  MyMessage mMotionArmedMsg = mMotionArmedMsg(mBoulderId, V_ARMED);
  MyMessage mMotionTrippedMsg = mMotionTrippedMsg(mBoulderId, V_TRIPPED);
}

void Communication::setupCommunication()
{
  mGw.begin(onIncomingMessageEvent, 0);
  mGw.sendSketchInfo(firmwareName,firmwareVer);
  mGw.present(mBoulderId,S_ARDUINO_NODE);
}

void Communication::updateCommunication()
{
  //Update Radio
  mGw.process();
}

