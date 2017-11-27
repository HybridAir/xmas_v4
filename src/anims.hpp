#ifndef ANIMS_HPP
#define ANIMS_HPP

#include "main.hpp"


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))                              // array size macro


uint8_t gHue = 0;                                                               // variable hue value used by many animations
uint8_t currentAnimIndex = 0;                                                   // the index of the currently playing animation
uint8_t cometPos = 0;                                                           // used in the cometCW() animation
bool firstSnow = true;                                                          // used in the snow() animation


void sequentialRGB();
void glitterRGB();
void fullRGB();
void comet();
void glitterRandom();
void snow();
void letItSnow();
void xmasWave();
void randXmasWave();


class Sin8XmasLed {
    uint8_t ledIndex;
    uint16_t sinOffset;
    uint8_t currentColor;
    uint8_t tempColor = 0;
    uint8_t prevBrightness = 0;
    bool decreasing = false;
public:
    Sin8XmasLed(uint8_t _ledIndex, uint16_t _sinOffset);
    void randColor();
    void runAnim(CRGB* _ledArray);
};


typedef void (*AnimList[])();                                                   // array to hold all the animation fucntions

// animation function array
AnimList animations = {randXmasWave, sequentialRGB, glitterRGB, xmasWave, fullRGB, comet, glitterRandom, snow};

#endif
