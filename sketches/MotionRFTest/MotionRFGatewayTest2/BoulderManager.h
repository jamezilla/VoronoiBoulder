#define NUM_BOULDERS 6

class BoulderManager {

  struct BoulderInfo {
    bool calibrated = false;
  };

  public:
    BoulderManager();

    BoulderInfo mBoulders[NUM_BOULDERS];
  
}

