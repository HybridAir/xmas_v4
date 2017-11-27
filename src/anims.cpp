// xmas_v4 HybridAir 2017
// handles all led animation related stuff

#include "anims.hpp"


// displays the currently selected animation
void runAnim() {
    animations[currentAnimIndex]();                                             // call the currently selected animation function to update the led array
    FastLED.show();                                                             // send the new led array out to the actual LED strip
    FastLED.delay(1000/FRAMES_PER_SECOND);                                      // framerate delay
}


// switches to the next animation listed in AnimList animations
void nextAnim() {
    currentAnimIndex = (currentAnimIndex + 1) % ARRAY_SIZE(animations);         // increment the current animation index, and wrap around to 0 if we're at the end
    firstSnow = true;
    FastLED.clear();                                                            // clear the leds
    FastLED.show();
}


// returns the current animation index value
uint8_t currentAnim() {
    return currentAnimIndex;
}


// sets current animation index to the specified input value
void setAnim(uint8_t input) {
    if(input >= ARRAY_SIZE(animations)) {                                       // sanity check
        currentAnimIndex = 0;                                                   // default to animation 0 if the input animation is out of range
    }
    else {
        currentAnimIndex = input;
    }
    FastLED.clear();                                                            // clear the leds
    FastLED.show();
}


// ANIMATIONS ==================================================================


// fades through the rainbow through each led sequentially
void sequentialRGB() {
    fill_rainbow( leds, NUM_LEDS, gHue, 20);
    gHue++;
}


// a glittery, semi-random animation that cycles through the rainbow
void glitterRGB() {

    fadeToBlackBy( leds, NUM_LEDS, 4);                                          // fade all leds out over time until they get randomly updated, by 4/255 (about 1.5%) every cycle

    // every 75 ms, set a random led to a new random color within 24 of the current gHue
    EVERY_N_MILLISECONDS( 75 ) {
        uint8_t pos = random8(NUM_LEDS);                                        // select a random led position
        leds[pos] += CHSV( gHue + random8(24), 255, 255);                       // give that random led a random hue at full brightness
    }
    gHue++;
}


// fades through the rainbow on all leds at the same time
void fullRGB() {
    // sequentially fills the led array with a rainbow starting from the specified offset
    fill_solid( leds, NUM_LEDS, CHSV( gHue, 255, 255));
    gHue++;
}


// a sequentitially lit led moving clockwise, followed by a fading trail
void comet() {
    fadeToBlackBy( leds, NUM_LEDS, 10);                                         // make the trail fade out over time

    EVERY_N_MILLISECONDS( 300 ) {
        cometPos = (cometPos + 1) % NUM_LEDS;                                   // every 300 ms, move the leading led up by 1, and roll over to 0
        gHue++;
    }

    leds[cometPos] = CHSV( gHue, 255, 255);                                    // set a new led at full brightness with the newest hue at the specified position
}


// like the other glitter animation, but with randomly selected colors
void glitterRandom() {
    fadeToBlackBy( leds, NUM_LEDS, 4);

    // every 75 ms, set a random led to a new random color within 24 of the current gGue
    EVERY_N_MILLISECONDS( 75 ) {
        uint8_t pos = random8(NUM_LEDS);                                        // get a random led
        leds[pos] += CHSV( random8(), 255, 255);                                // give that random led a random color at full brightness
    }
}


// a calm, random, white led fading animation, meant to simulate snowfall or something
void snow() {
    if(firstSnow) {
        firstSnow = false;
        letItSnow();                                                            // skip the delay on the first run, so the user knows that the device is still functioning
    }

    EVERY_N_MILLISECONDS( 40 ) {
        fadeToBlackBy( leds, NUM_LEDS, 1);                                      // fade out all leds very slowly
    }

    EVERY_N_MILLISECONDS( 1250 ) {                                              // display new leds every 1250 ms
        letItSnow();
    }
}


// used by the snow animation, displays between 1 and 4 white leds in random positions
void letItSnow() {
    for(uint8_t i = 0; i < random8(1, 4); i++) {                                // display a random amount (within 1 and 4) of leds
        leds[random8(NUM_LEDS)] += CHSV( 0, 0, 128);                            // set a random led to white at mid brightness
    }
}



// fades 3 different color sections using offset sine wave functions
void xmasWave() {

    // sine wave generators for each color section (red, green, and blue in this case (since they're xmas colors idk))
    uint8_t r = beatsin8( 60, 0, 255 );
    uint8_t g = beatsin8( 60, 0, 255, 0, 85);
    uint8_t b = beatsin8( 60, 0, 255, 0, 170);

    uint8_t colorSection = 0;                                                   // counter used for alternating led colors
    for(uint8_t i = 0; i < NUM_LEDS; i++) {                                     // for each led in the strip
        switch(colorSection) {                                                  // display the leds by section (red, then green, then blue, repeating)
            case 0:
                leds[i] = CRGB( r, 0, 0);
                break;
            case 1:
                leds[i] = CRGB( 0, g, 0);
                break;
            case 2:
                leds[i] = CRGB( 0, 0, b);
                break;
        }
        colorSection = (colorSection + 1) % 3;                                  // loop the colorSection counter back to 0
    }
}


// class used to fade a single led in any position and sine wave offset, while randomly changing to a preset color each cycle
Sin8XmasLed::Sin8XmasLed(uint8_t _ledIndex, uint16_t _sinOffset) {
    ledIndex = _ledIndex;
    sinOffset = _sinOffset;
    randColor();                                                                // start at a random color
}


// select a random color preset (red green or blue because xmas colors you know)
void Sin8XmasLed::randColor() {
    switch(random8(3)) {
        case 0:
            currentColor = HUE_RED;
            break;
        case 1:
            currentColor = HUE_GREEN;
            break;
        case 2:
            currentColor = HUE_BLUE;
            break;
    }
}


// displays the led animation, feed it your led array
void Sin8XmasLed::runAnim(CRGB* ledArray) {
    uint8_t brightness = beatsin8( 60, 0, 255, 0, sinOffset);                   // the sine wave function is used to modulate the led's brightness
    if(brightness < prevBrightness) {                                           // check if the brightness is decreasing so we know when to change colors
        decreasing = true;
    }
    else if(decreasing) {                                                       // if the brightness has finished decreasing, it means the cycle has ended so change colors
        decreasing = false;
        randColor();
    }
    prevBrightness = brightness;

    ledArray[ledIndex] = CHSV( currentColor, 255, brightness);                  // display the new led
}


// a Sin8XmasLed object for each led in the strip
Sin8XmasLed led0(0, 0);
Sin8XmasLed led1(1, 90);
Sin8XmasLed led2(2, 180);
Sin8XmasLed led3(3, 270);
Sin8XmasLed led4(4, 315);
Sin8XmasLed led5(5, 225);
Sin8XmasLed led6(6, 135);
Sin8XmasLed led7(7, 45);


//like xmasWave but inetad of preset color "positons", they are random
void randXmasWave() {
    led0.runAnim(leds);
    led1.runAnim(leds);
    led2.runAnim(leds);
    led3.runAnim(leds);
    led4.runAnim(leds);
    led5.runAnim(leds);
    led6.runAnim(leds);
    led7.runAnim(leds);
}
