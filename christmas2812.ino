#include <TuyaWifi.h>
#include <FastLED.h>
#include "Cube2812.h"

#define SOFTSERIAL_DEBUG 1

#ifdef SOFTSERIAL_DEBUG
#include <SoftwareSerial.h>
#endif

// int mode = 0; // Currently-active animation mode, 0-9
#ifdef SOFTSERIAL_DEBUG
SoftwareSerial debugSerial(12, 13);
#endif

TuyaWifi my_device;

unsigned char led_state = 0;
/* Connect network button pin */

const uint8_t WIFI_RECONNECT_BUTTON_PIN = 3;


/******************************************************************************
                        1:dp数据点序列号重新定义
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
//开关(可下发可上报)
//备注:
#define DPID_SWITCH_LED 20
//模式(可下发可上报)
//备注:
#define DPID_WORK_MODE 21
//亮度值(可下发可上报)
//备注:
#define DPID_BRIGHT_VALUE 22
//冷暖值(可下发可上报)
//备注:
#define DPID_TEMP_VALUE 23
//彩光(可下发可上报)
//备注:类型：字符；
//Value: 000011112222；
//0000：H（色度：0-360，0X0000-0X0168）；
//1111：S (饱和：0-1000, 0X0000-0X03E8）；
//2222：V (明度：0-1000，0X0000-0X03E8)
#define DPID_COLOUR_DATA 24
//倒计时(可下发可上报)
//备注:
#define DPID_COUNTDOWN 26
//音乐律动(只下发)
//备注:类型：字符串；
//Value: 011112222333344445555；
//0：   变化方式，0表示直接输出，1表示渐变；
//1111：H（色度：0-360，0X0000-0X0168）；
//2222：S (饱和：0-1000, 0X0000-0X03E8）；
//3333：V (明度：0-1000，0X0000-0X03E8）；
//4444：白光亮度（0-1000）；
//5555：色温值（0-1000）
#define DPID_MUSIC_DATA 27
//灯带长度(只上报)
//备注:幻彩灯带在UI面板上展示的总长度
#define DPID_LIGHT_LENGTH 46
//灯带点数(只上报)
//备注:幻彩灯带驱动芯片总数
#define DPID_LIGHT_PIXEL 47
//幻彩情景(可下发可上报)
//备注:专门用于幻彩灯带场景
#define DPID_DREAMLIGHT_SCENE_MODE 51
//幻彩本地音乐律动(可下发可上报)
//备注:专门用于幻彩灯带本地音乐律动
#define DPID_DREAMLIGHTMIC_MUSIC_DATA 52
//点数/长度设置(可下发可上报)
//备注:幻彩灯带裁剪之后，对长度重新进行设置
#define DPID_LIGHTPIXEL_NUMBER_SET 53
//涂抹调色(可下发可上报)
//备注:专门用于幻彩灯带的调光调色
#define DPID_PAINT_COLOUR_DATA 61


///* Current device DP values */
unsigned char dp_bool_value = 0;
long dp_value_value = 0;
unsigned char dp_enum_value = 0;
unsigned char dp_string_value[21] = {"0"};
uint16_t Hue = 0; //HSV
uint8_t Sat = 0;
uint8_t Val = 0;
uint8_t scene_mode = 0;
unsigned char hex[10] = {"0"};
//unsigned char dp_raw_value[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
//int dp_fault_value = 0x01;

/* Stores all DPs and their types. PS: array[][0]:dpid, array[][1]:dp type. 
 *                                     dp type(TuyaDefs.h) : DP_TYPE_RAW, DP_TYPE_BOOL, DP_TYPE_VALUE, DP_TYPE_STRING, DP_TYPE_ENUM, DP_TYPE_BITMAP
*/
/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
unsigned char dp_array[][2] =
{
  {DPID_SWITCH_LED, DP_TYPE_BOOL},
  {DPID_WORK_MODE, DP_TYPE_ENUM},
  {DPID_BRIGHT_VALUE, DP_TYPE_VALUE},
  {DPID_TEMP_VALUE, DP_TYPE_VALUE},
  {DPID_COLOUR_DATA, DP_TYPE_STRING},
  {DPID_COUNTDOWN, DP_TYPE_VALUE},
  {DPID_MUSIC_DATA, DP_TYPE_STRING},
  {DPID_LIGHT_LENGTH, DP_TYPE_VALUE},
  {DPID_LIGHT_PIXEL, DP_TYPE_VALUE},
  {DPID_DREAMLIGHT_SCENE_MODE, DP_TYPE_RAW},
  {DPID_DREAMLIGHTMIC_MUSIC_DATA, DP_TYPE_RAW},
  {DPID_LIGHTPIXEL_NUMBER_SET, DP_TYPE_VALUE},
  {DPID_PAINT_COLOUR_DATA, DP_TYPE_RAW},
};

unsigned char pid[] = {"v9pudsuphqwpqbqa"}; //*********处替换成涂鸦IoT平台自己创建的产品的PID
unsigned char mcu_ver[] = {"1.0.0"};

void setup()
{
    pinMode(WIFI_RECONNECT_BUTTON_PIN, INPUT_PULLUP);
#ifdef SOFTSERIAL_DEBUG
    debugSerial.begin(9600);
    debugSerial.print(F("WIFI_UART_RECV_BUF_LMT: "));
    debugSerial.println(WIFI_UART_RECV_BUF_LMT);
    debugSerial.print(F("WIFI_DATA_PROCESS_LMT: "));
    debugSerial.println(WIFI_DATA_PROCESS_LMT);
#endif
    Serial.begin(9600);
    //Initialize led port, turn off led.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    //Initialize networking keys.
    //incoming all DPs and their types array, DP numbers
    //Enter the PID and MCU software version
    my_device.init(pid, mcu_ver);
    my_device.set_dp_cmd_total(dp_array, 17);
    //register DP download processing callback function
    my_device.dp_process_func_register(dp_process);
    //register upload all DP callback function
    my_device.dp_update_all_func_register(dp_update_all);

    initCube2812();
    // setRenderMode(SNOW_FLURRY);
}

void loop()
{
    static unsigned long last_time = 0;
    //Enter the connection network mode when Pin7 is pressed.
    if (digitalRead(WIFI_RECONNECT_BUTTON_PIN) == LOW)
    {
        delay(80);
        if (digitalRead(WIFI_RECONNECT_BUTTON_PIN) == LOW)
        {
            my_device.mcu_set_wifi_mode(SMART_CONFIG);
        }
    }
    my_device.uart_service();

    /* LED blinks when network is being connected */
    if ((my_device.mcu_get_wifi_work_state() != WIFI_LOW_POWER) && (my_device.mcu_get_wifi_work_state() != WIFI_CONN_CLOUD) && (my_device.mcu_get_wifi_work_state() != WIFI_SATE_UNKNOW))
    {
        if (millis() - last_time >= 500)
        {
            last_time = millis();

            if (led_state == LOW)
            {
                led_state = HIGH;
            }
            else
            {
                led_state = LOW;
            }
            digitalWrite(LED_BUILTIN, led_state);
        }
    }

    updateCube2812();
}

/**
 * @description: DP download callback function.
 * @param {unsigned char} dpid
 * @param {const unsigned char} value
 * @param {unsigned short} length
 * @return {unsigned char}
 */
unsigned char dp_process(unsigned char dpid, const unsigned char value[], unsigned short length)
{
#ifdef SOFTSERIAL_DEBUG    
    debugSerial.print(F("dpid: "));
    debugSerial.println(dpid);
#endif
    switch (dpid)
    {
    case DPID_SWITCH_LED:
        dp_bool_value = my_device.mcu_get_dp_download_data(dpid, value, length); /* Get the value of the down DP command */
        if (dp_bool_value)
        {
            //Turn on
            turnOnCube2812();
        }
        else
        {
            //Turn off
            turnOffCube2812();
        }

        //Status changes should be reported.
        my_device.mcu_dp_update(dpid, value, length);
        break;

    case DPID_BRIGHT_VALUE:
        dp_value_value = my_device.mcu_get_dp_download_data(dpid, value, length); /* Get the value of the down DP command */

        FastLED.setBrightness(dp_value_value);
        my_device.mcu_dp_update(dpid, value, length);
        break;

    case DPID_WORK_MODE:
        dp_enum_value = my_device.mcu_get_dp_download_data(dpid, value, length); /* Get the value of the down DP command */
        switch (dp_enum_value)
        {
        case 0: // white mode
            break;
        case 1: // colour mode

            break;
        case 2: // scene mode

            break;
        case 3: // music mode

            break;
        }
        //Status changes should be reported.
        my_device.mcu_dp_update(dpid, value, length);
        break;

    case DPID_COUNTDOWN: //倒计时
        my_device.mcu_dp_update(dpid, value, length);
        break;

    case DPID_MUSIC_DATA: //音乐律动
        my_device.mcu_dp_update(dpid, value, length);
        break;

    // case DPID_SCENE_DATA:
    //     my_device.mcu_dp_update(DPID_SCENE_DATA, value, length);
    //     scene_mode = value[1];

    //     switch (scene_mode)
    //     {
    //     case 0:
    //         break;
    //     case 1:
    //         break;
    //     case 2:
    //         break;
    //     case 3:
    //         break;
    //     case 4:
    //         break;
    //     case 5:
    //         break;
    //     case 6:
    //         break;
    //     case 7:
    //         break;
    //     case 8:
    //         break;
    //     }
    //     break;

    case DPID_DREAMLIGHT_SCENE_MODE: //炫彩情景
        my_device.mcu_dp_update(DPID_DREAMLIGHT_SCENE_MODE, value, length);
        scene_mode = value[1];
#ifdef SOFTSERIAL_DEBUG        
        debugSerial.print(F("mode value: "));
        debugSerial.println(scene_mode);
#endif
        switch (scene_mode)
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;

        case 201: // 自定义的第一个
            setRenderMode(SUN);
            break;
        case 202: // 自定义的第一个
            setRenderMode(SNOW);
            break;
        case 203: // 自定义的第一个
            setRenderMode(SNOW_FLURRY);
            break;
        // case 203: // 自定义的第一个
        //     setRenderMode(HACKER_MATRIX);
        //     break;
        // case 204: // 自定义的第一个
        //     setRenderMode(RAIN);
        //     break;
        // case 205: // 自定义的第一个
        //     setRenderMode(BUBBLE);
        //     break;
        // case 206: // 自定义的第一个
        //     setRenderMode(RAINBOW);
        //     break;
        // case 207: // 自定义的第一个
        //     setRenderMode(ENERGY_CUBE);
        //     break;
        // case 208: // 自定义的第一个
        //     setRenderMode(SNAKE_GAME);
        //     break;

        // APP 只能定义 8 个自定义场景，所以贪吃蛇另外做一个
        // case 200: // 自定义的第一个
        //     setRenderMode(ENERGY_CUBE);
        //     break;
        // case 201: // 自定义的第一个
        //     setRenderMode(SNAKE_GAME);
        //     break;
        // case 202: // 上
        //     inputDir(UP);
        //     break;
        // case 203: // 右
        //     inputDir(RIGHT);
        //     break;
        // case 204: // 左
        //     inputDir(LEFT);
        //     break;
        // case 205: // 下
        //     inputDir(DOWN);
        //     break;
        // case 206: // 自定义的第一个
        //     setRenderMode(COLOURFUL_DREAM);
        //     break;
        // case 207: // 自定义的第一个
        //     setRenderMode(STAR_SKY);
        //     break;

        }

        break;

    case DPID_LIGHTPIXEL_NUMBER_SET: //长度设置
        my_device.mcu_dp_update(dpid, value, length);
        break;
    default:
        break;
    }
    return SUCCESS;
}

/**
 * @description: Upload all DP status of the current device.
 * @param {*}
 * @return {*}
 */
void dp_update_all(void)
{
    my_device.mcu_dp_update(DPID_SWITCH_LED, led_state, 1);
}
