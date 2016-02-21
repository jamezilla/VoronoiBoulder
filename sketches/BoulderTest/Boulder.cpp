#include "Boulder.h"

rgb hexToRGB(String hex) {

  rgb color;

  // Get rid of '#' and convert it to integer
  long number = strtol( &hex[0], NULL, 16);

  // Split them up into r, g, b values
  color.r = number >> 16;
  color.g = number >> 8 & 0xFF;
  color.b = number & 0xFF;

  return color;
}


Boulder::Boulder() {

}

void Boulder::setBoulderColor(String hex) {
  mColor = hexToRGB(hex);
}

rgb Boulder::getBoulderColor() {
  return mColor;
}


