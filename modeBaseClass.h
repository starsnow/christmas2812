// modeBaseClass.h
// seesea 2021-07-09
// 模式的基类

#ifndef _MODE_BASE_CLASS_H_
#define _MODE_BASE_CLASS_H_

class RenderMode
{
public:
    virtual String       getName() {};           // 模式名字
    virtual void         render() {};            // 渲染函数
    virtual unsigned int getRenderInterval() {}; // 渲染时间间隔（单位毫秒）
    virtual void         init() {};              // 初始化
    virtual void         input(uint8_t dir) {};  // 交互的输入
};

#endif
