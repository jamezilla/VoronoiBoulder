#include <FastLED.h>
#include "config.h"

class Nodule {

public:
  typedef uint8_t Hue;
  typedef uint8_t Saturation;
  typedef uint8_t Value;

  typedef enum HueMode {
    HueModePerlin,
    HueModeRainbow
  } HueMode;

  typedef enum SaturationMode {
    SaturationModeConstant
  } SaturationMode;

  typedef enum ValueMode {
    ValueModeConstant
  } ValueMode;

  Nodule();
  Nodule(CRGB *leds);

  void setHue(Hue hue);
  void setHueUpdateInterval(uint16_t interval);
  void setHueMode(HueMode mode);

  void setSaturation(Saturation saturation);
  void setSaturationUpdateInterval(uint16_t interval);
  void setSaturationMode(SaturationMode mode);

  void setValue(Value value);
  void setValueUpdateInterval(uint16_t interval);
  void setValueMode(ValueMode mode);

  void update();

private:
  void updateHue(uint32_t &elapsed);
  void updateLeds();
  void updatePerlin();
  void updateRainbow();
  void updateSaturation(uint32_t &elapsed);
  void updateValue(uint32_t &elapsed);

  CRGB           *leds;                     // our slice of the LED array
  uint32_t       previousTime;              // the time of our last update

  Hue            hue;
  HueMode        hueMode;
  uint16_t       hueUpdateInterval;

  Saturation     saturation;
  SaturationMode saturationMode;
  uint16_t       saturationUpdateInterval;

  Value          value;
  ValueMode      valueMode;
  uint16_t       valueUpdateInterval;

  // Perlin noise
  uint16_t       noiseSeed;                 // where do we start in the noise field?
  float          noiseSpeedAccumulator;     // keep a rolling tally of how far we've gone
  float          noiseSpeedIncrement;       // how much to add on each iteration

};
