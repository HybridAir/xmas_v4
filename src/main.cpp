// xmas_v4 HybridAir 2017
// designed for an attiny85

#include "main.hpp"


// HACKY FIX AHEAD
// this stuff is needed to keep the compiler from screaming
// in reference to https://www.reddit.com/r/arduino/comments/4vdr3b/neopixel_goggles_with_attiny85/d5y8z3q/
// do this too: https://github.com/FastLED/FastLED/issues/413
// refer to this maybe: https://github.com/FastLED/FastLED/issues/485
extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
extern "C" void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));
void __cxa_pure_virtual(void) {
    abort();
}
void __cxa_deleted_virtual(void) {
    abort();
}
FASTLED_USING_NAMESPACE


CRGB leds[NUM_LEDS];                                                            // an array to hold all of the leds and their rgb color values
bool prevBtn = false;                                                           // the last recorded button state
uint32_t btnTimer = 0;                                                          // timer used to measure button hold time and debouncing


void setup() {
    DDRB = _BV(POWER_PIN) | _BV(PB0);                                           // set the power pin and test led to output mode
    PORTB |= _BV(POWER_PIN);                                                    // default the power pin to high, do this asap
    PORTB |= _BV(BUTTON_PIN);                                                   // enable the internal pullup on the button pin

    // set up fastled
    FastLED.addLeds<NEOPIXEL,LED_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);                                          // set the limited master led brightness
    setAnim(EEPROM.read(0));                                                    // get the last played animation out of the eeprom
}


void loop() {
    runAnim();                                                                  // display the led aniamtion
    checkButton();                                                              // check the button for updates

    if(millis() >= AUTO_SHUTDOWN_DELAY) {                                       // if the current time is greater than the auto shutdown delay value
        shutdown();
    }
}


// saves the currently playing animation and shuts the device down
void shutdown() {
    FastLED.clear();                                                            // turn the leds off
    FastLED.show();
    EEPROM.write(0, currentAnim());                                             // save the current animation to the eeprom
    delay(1000);                                                                // time to stabilize

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);                                        // set the sleep mode
    sleep_enable();                                                             // sets the Sleep Enable bit in the MCUCR Register (SE BIT)

    // turn off the power and button pullup pins
    DDRB = 0;
    PORTB = 0;
    sleep_cpu();                            // sleep (just in case)
    delay(1000);                                                                // delay here to prevent the leds from coming back while the latch turns off
}


// monitors button presses, measures held duration, and processes accordingly
void checkButton() {
    bool newBtn = !(PINB & _BV(BUTTON_PIN));                                    // button input is active LOW, so invert here to make the following logic easier

    if(newBtn != prevBtn) {                                                     // if the button state has changed
        prevBtn = newBtn;                                                       // record the new button state

        if(newBtn) {                                                            // if the button was just pressed
            btnTimer = millis();                                                // start the hold timer to determine what function the user wants to activate
        }
        else {                                                                  // if the button was just released
            if(millis() >= btnTimer + SLEEP_DELAY) {                            // and if it was held for SLEEP_DELAY ms
                // go to sleep here

                //FastLED.clear();                                                            // turn the leds off
                //FastLED.show();
                //EEPROM.write(0, currentAnim());                                             // save the current animation to the eeprom

                //while(!(PINB & _BV(BUTTON_PIN))) {}


                shutdown();
            }
            else if(millis() >= btnTimer + ANIM_DELAY) {                        // if the button was only held for at least ANIM_DELAY ms (essentially a small debounce)
                nextAnim();                                                     // switch animations
            }
        }
    }
}
