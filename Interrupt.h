#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "Sys.h"

// 中断委托（中断号，参数）
typedef void (*InterruptCallback)(ushort num, void* param);

// 中断管理类
class TInterrupt
{
private:
#ifdef STM32F10X
    __IO uint _mem[84 + 128 / 4];  // 中断向量表要求128对齐，这里多分配128字节，找到对齐点后给向量表使用
#endif
    __IO uint* _Vectors; 	// 真正的中断向量表
    uint Vectors[84];   	// 对外的中断向量表
    uint Params[84];    	// 每一个中断向量对应的参数

    static void OnHandler();

public:
    void Init();    // 初始化中断向量表
		
    bool Activate(short irq, InterruptCallback isr, void* param = NULL);// 注册中断函数
																																				// irq终端号			isr中断函数		param中断函数
    bool Deactivate(short irq);		// 解除中断注册
    bool Enable(short irq);				// 开中断
    bool Disable(short irq);			// 关中断

    bool EnableState(short irq);	// 取消挂起
    bool PendingState(short irq);	// 挂起

    void SetPriority(short irq, uint priority);		// 设置优先级
    void GetPriority(short irq);									// 获取优先级
#ifdef STM32F10X
    uint EncodePriority (uint priorityGroup, uint preemptPriority, uint subPriority);		 // 修改优先级
    void DecodePriority (uint priority, uint priorityGroup, uint* pPreemptPriority, uint* pSubPriority);	// 读取优先级
#endif
};

extern TInterrupt Interrupt;		// 全局中断类对象    会在maim（）  之前进行初始化   没有构造函数... 
																// 初始化函数  Interrupt.Init（）  在 Sys.cpp 内 TSys::TSys() 中被调用
																// <TSys::TSys()也是构造函数   Sys.TSys()函数 在main（）函数之前被执行>
#endif
