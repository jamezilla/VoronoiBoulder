#pragma once

//
// Generators.h
//

#include "Nodule.h"

class Generators {
public:

  // generators

  static void perlin(Nodule::Channel &channel);
  static void reverseSawtooth(Nodule::Channel &channel);
  static void sinusoidal(Nodule::Channel &channel);
  static void sawtooth(Nodule::Channel &channel);

  // helpers

  static void jumpToSineLutPosition(Nodule::Channel &channel);

};
