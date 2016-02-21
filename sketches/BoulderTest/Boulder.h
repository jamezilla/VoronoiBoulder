#include "Arduino.h"

struct rgb {
  long r, g, b;
};

class Boulder {
  public:

    Boulder();

    void setBoulderColor(String hex);
    rgb  getBoulderColor();

    rgb mColor;
};

