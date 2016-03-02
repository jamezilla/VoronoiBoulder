#include "Generators.h"
#include <avr/pgmspace.h>

// The max gap between values in the sine LUT is 4, so any random value can only
// be 2 away from a value in the table
#define SINE_LUT_MAX_GAP 2U

const PROGMEM uint8_t sineLUT[] = {
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,
  187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,
  233,234,236,238,239,240,242,243,244,245,247,248,249,249,250,251,252,252,253,253,
  253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,
  244,243,242,240,239,238,236,234,233,231,229,227,225,223,221,219,217,215,212,210,
  208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,
  152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,
  87,84,81,78,76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,
  21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,
  5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,33,35,37,39,42,44,46,49,51,54,
  56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
};


// move channel.lutIndex to the first position in the lookup table which is within
// SINE_LUT_MAX_GAP of the current value - use this to avoid a big jump in value
// when switching to sinusoidal modes
void Generators::jumpToSineLutPosition(Nodule::Channel &channel) {
  for (int i = 0; i < 256; i++) {
    if (abs(channel.value - pgm_read_byte_near(sineLUT + i)) <= SINE_LUT_MAX_GAP) {
      channel.lutIndex = i;
      break;
    }
  }
}

// sine wave generator
void Generators::sinusoidal(Nodule::Channel &channel) {
  // update our index into the lookup table
  // NOTE: we let this intentionally overflow to wrap around to the beginning of
  // the table
  channel.lutIndex += 1;

  // store the value
  channel.value = sineLUT[channel.lutIndex];
}


// perlin noise - move randomly, but smoothly from one value to the next
void Generators::perlin(Nodule::Channel &channel) {
  channel.noiseSeed += 1;

  // inoise8() always returns the same value for a given seed
  channel.value = inoise8(channel.noiseSeed);
}


// a linear ramp, which goes from 255 to 0, and wraps back to 255
void Generators::reverseSawtooth(Nodule::Channel &channel) {
  if (channel.value == 0) {
    channel.value = UINT8_MAX;
  } else {
    channel.value -= 1;
  }
}


// a linear ramp, which leverages integer overflow to wrap back to zero
void Generators::sawtooth(Nodule::Channel &channel) {
  channel.value += 1;
}
