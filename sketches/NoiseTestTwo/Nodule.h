#include <FastLED.h>
#include "config.h"

class Nodule {

public:
  typedef uint8_t Hue;
  typedef uint8_t Saturation;
  typedef uint8_t Value;

  typedef enum HueMode {
    Perlin,
    Rainbow
  } HueMode;

  typedef enum SaturationMode {
    Constant
  } SaturationMode;

  Nodule();
  Nodule(CRGB *leds);

  void setMode(Mode mode);
  void setSaturationMode(SaturationMode saturationMode);
  void setUpdateInterval(uint16_t updateInterval);
  void update();

private:
  void updateLeds();
  void updatePerlin();
  void updateRainbow();

  CRGB       *leds;
  HueMode    hueMode;
  uint16_t   updateInterval;
  uint32_t   previousTime;
  Hue        hue;
  Saturation saturation;
  Value      value;

  // Perlin noise
  uint16_t   noiseSeed;
  float      noiseSpeedAccumulator;
  float      noiseSpeedIncrement;

};
