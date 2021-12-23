// snowFlurryMode.h
// seesea 2021-12-23
// 风雪模式

#ifndef _SNOW_FLURRY_MODE_H_
#define _SNOW_FLURRY_MODE_H_

#include "Cube2812.h"
#include "modeBaseClass.h"

// FastLED 的 LED 数据
// 内存不够用，只用三个面
extern CRGB leds[MATRIX_BUFFER_NUM][NUM_LEDS_PER_MATRIX];

// 各个 FastLED 控制器
extern CLEDController* FastLEDControllers[MATRIX_NUM];

class SnowFlurryMode : public RenderMode
{
private:
    static const uint8_t SNOW_NUM = 15;
    static const uint8_t MAX_WINDSPEED = 3;
    
    unsigned int renderInterval;
    uint8_t newDotChance;
    uint8_t brightnessDelta;
    int8_t  windspeed;          // 风力，每个帧的时候只略微增减一下风力，这样风力会比较平衡
    Point snows[SNOW_NUM];
    CRGB *pLedsTop;

public:
    SnowFlurryMode()
    {
        renderInterval = 100;
        newDotChance = 20;
        brightnessDelta = 30;
        windspeed = 0;
    }

    String getName() 
    {
        return F("snow flurry mode");
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
    
    // 飞雪效果渲染
    void renderSnowFlurry(CRGB pLeds[], uint8_t dir)
    {
        const CRGB SNOW_COLOR = CRGB::White;
        CRGB *pLed;
        uint8_t i;

        if (random(100) < 2)
        {
            windspeed += random(3) - 1;
            if (windspeed > MAX_WINDSPEED)
                windspeed = MAX_WINDSPEED;
            else if (windspeed < -MAX_WINDSPEED)
                windspeed = -MAX_WINDSPEED;
        }

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

            // 加上风导致的横向偏移，为了避免雪飘没了，就让超出范围的从另一边再出现
            snows[i].x = (snows[i].x + windspeed + 8) % 8;

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
        renderSnowFlurry(pLedsTop, CW0);
    }

    void input(uint8_t)
    {
        
    }
};

#endif