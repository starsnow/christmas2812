// snowMode.h
// seesea 2021-12-23
// 飘雪模式

#ifndef _SNOW_MODE_H_
#define _SNOW_MODE_H_

#include "cube2812.h"
#include "modeBaseClass.h"

// FastLED 的 LED 数据
// 内存不够用，只用三个面
extern CRGB leds[MATRIX_BUFFER_NUM][NUM_LEDS_PER_MATRIX];

// 各个 FastLED 控制器
extern CLEDController* FastLEDControllers[MATRIX_NUM];

class SnowMode : public RenderMode
{
private:
    static const uint8_t SNOW_NUM = 15;

    unsigned int renderInterval;
    uint8_t newDotChance;
    uint8_t brightnessDelta;
    Point snows[SNOW_NUM];
    CRGB *pLedsTop;

public:
    SnowMode()
    {
        renderInterval = 100;
        newDotChance = 20;
        brightnessDelta = 30;
    }

    String getName() 
    {
        return F("snow mode");
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
    
    // 飘雪效果渲染
    renderSnow(CRGB pLeds[], uint8_t dir)
    {
        const CRGB SNOW_COLOR = CRGB::White;
        CRGB *pLed;
        uint8_t i;

        // 向下移动
        for (i = 0; i < SNOW_NUM; ++i)
        {
            if (snows[i].x == -1)
                continue;

            pLeds[XY(snows[i].x, snows[i].y, dir)] = CRGB::Black;

            snows[i].y++;
            if (snows[i].y >= 8)
            {
                snows[i].x = -1;
                continue;
            }
            pLeds[XY(snows[i].x, snows[i].y, dir)] = SNOW_COLOR;
        }

        // 根据几率来生成一个雪花
        if (random8(100) > (100 - newDotChance))
            return;

        for (i = 0; i < SNOW_NUM; ++i)
        {
            // 找到空余头部点位置的时候才生成新的头部点
            if (snows[i].x != -1)
                continue;

            snows[i].x = random8(MATRIX_WIDTH);
            snows[i].y = 0;
            break;
        }
    }

    void render() 
    {
        renderSnow(pLedsSide, CW0);
    }

    void input(uint8_t)
    {
        
    }
};

#endif