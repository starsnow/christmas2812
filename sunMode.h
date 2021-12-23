// sunMode.h
// seesea 2021-12-23
// 暖阳模式

#ifndef _SUN_MODE_H_
#define _SUN_MODE_H_

#include "Cube2812.h"
#include "modeBaseClass.h"

// FastLED 的 LED 数据
// 内存不够用，只用三个面
extern CRGB leds[MATRIX_BUFFER_NUM][NUM_LEDS_PER_MATRIX];

// 各个 FastLED 控制器
extern CLEDController* FastLEDControllers[MATRIX_NUM];

class SunMode : public RenderMode
{
private:
    unsigned int renderInterval;
    uint8_t brightnessDelta;
    CRGB *pLedsTop;

public:
    SunMode()
    {
        renderInterval = 100;
        brightnessDelta = 30;
    }

    String getName() 
    {
        return F("sun mode");
    }

    unsigned int getRenderInterval()
    {
        return renderInterval;
    }

    void init()
    {
        FastLEDControllers[UP_SIDE]->setLeds(leds[UP_SIDE], NUM_LEDS_PER_MATRIX);
        pLedsTop    = leds[UP_SIDE];
    }
    
    // 暖阳效果渲染
    // 微调明亮度就好了
    renderSun(CRGB pLeds[], uint8_t dir)
    {
        const CRGB SUN_COLOR = CRGB::Oringe;
        CRGB *pLed;
        uint8_t i;

    }

    void render() 
    {
        renderSun(pLedsSide, CW0);
    }

    void input(uint8_t)
    {
        
    }
};

#endif