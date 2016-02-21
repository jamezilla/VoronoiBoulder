#include "..\MySensor\MySensor.h"
#include <SPI.h>


class Communication
{
  public:

    Communication(int bId,string firmwareName,string firmwareVer);

    void setupCommunication();
    void updateCommunication();

    int mBoulderId;
    string mFirmwareName,mFirmwareVer;

  private:
    
    MySensor mGw;
    MyMessage mMotionArmedMsg;
    MyMessage mMotionTrippedMsg;
    
};
