#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>
#include "FastLED.h"
#include <EEPROM.h>
#include <avr/sleep.h>


#define BUTTON_PIN PB1
#define POWER_PIN PB4
#define LED_PIN    3

#define BRIGHTNESS          32                                                  // max led brightness out of 255
#define FRAMES_PER_SECOND  60                                                   // animation framerate
#define COLOR_ORDER RGB                                                         // led color order
#define NUM_LEDS    8                                                           // number of leds in the "strip"

#define AUTO_SHUTDOWN_DELAY         3600000                                     // time in ms to wait until auto sleeping (1 hour, but is about 4 minutes slow)
#define ANIM_DELAY          20                                                  // ms the button needs to be held to change animations (essentially a debounce delay)
#define SLEEP_DELAY         1000                                                // ms the button needs to be held to enable sleep mode


uint8_t currentAnim();
void nextAnim();
void setAnim(uint8_t input);
void runAnim();
extern void shutdown();
extern void checkButton();


extern CRGB leds[NUM_LEDS];
extern bool prevBtn;
extern uint32_t btnTimer;


#endif
