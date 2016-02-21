#include "BoulderManager.h"

BoulderManager::BoulderManager() {


  
} 

void BoulderManager::setBoulderArmed(const int boulderId,bool armed) {
  mBoulders[boulderId] = armed;
}



