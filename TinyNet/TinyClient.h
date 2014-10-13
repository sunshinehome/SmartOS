﻿#ifndef __TinyClient_H__
#define __TinyClient_H__

#include "Sys.h"
#include "Message.h"

// 微网客户端
class TinyClient
{
private:
	Controller* _control;

public:
	byte	Server;		// 服务端地址
	ushort	DeviceType;	// 设备类型。两个字节可做二级分类
	ulong	Password;	// 通讯密码

	ulong	LastActive;	// 最后活跃时间
	
	TinyClient(Controller* control);

// 常用系统级消息
public:
	// 设置默认系统消息
	void SetDefault();

	// 广播发现系统
	void Discover();
	static bool OnDiscover(Message& msg, void* param);

	// Ping指令用于保持与对方的活动状态
	void Ping();
	static bool OnPing(Message& msg, void* param);

	// 询问及设置系统时间
	static bool SysTime(Message& msg, void* param);

	// 询问系统标识号
	static bool SysID(Message& msg, void* param);
};

#endif
