#pragma once

//
// Generators.h
//

#include "Cell.h"

class Generators {
public:

  // generators

  static void alternate(uint8_t &value);
  static void alternateDrift(uint8_t &value);
  static void perlin(uint8_t &value, uint16_t &noiseSeed);
  static void perlin(uint16_t &value, uint32_t &noiseSeed);
  static void reverseSawtooth(uint8_t &value);
  static void reverseSawtooth(uint16_t &value);
  static void sinusoidal(uint8_t &value, uint8_t &lutIndex);
  static void sinusoidal(uint16_t &value, uint8_t &lutIndex);
  static void sawtooth(uint8_t &value);
  static void sawtooth(uint16_t &value);

  // helpers

  static void jumpToSineLutPosition(uint8_t &value, uint8_t &lutIndex);
  static void jumpToSineLutPosition(uint16_t &value, uint8_t &lutIndex);

};
