#pragma once

#define PWRMGT                      // comment out to adjust brightness manually
#define VOLTAGE             5
#define MAXCURRENT          300

#define CHIP_WS281B                 // big boulder
// #define CHIP_APA102                 // small boulder

#define BRIGHTNESS          50

#define NUM_LEDS_PER_NODE   8
#define NUM_NODES           5
#define NUM_LEDS            40      // overall number of leds

#define MAX_SPEED           1.0f    // TODO: move this to a timer?
