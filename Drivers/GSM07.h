﻿#ifndef __GSM07_H__
#define __GSM07_H__

#include "Device\Port.h"

#include "App\AT.h"

#include "Net\Socket.h"
#include "Net\NetworkInterface.h"
#include "Net\ITransport.h"

#include "Message\DataStore.h"
#include "Message\Pair.h"

// GPRS的AT指令集 GSM 07.07
class GSM07 : public NetworkInterface
{
public:
	AT		At;		// AT操作对象
	cstring	APN;
	bool	Mux;	// 开启多路socket模式，最多同时开启4路

	IDataPort*	Led;	// 指示灯

	OutputPort	_Power;	// 电源
	OutputPort	_Reset;	// 重置
	OutputPort	_LowPower;	// 低功耗

	Delegate<Buffer&>	Received;

	//ushort	Country;	// 国家MCC 3位
	//ushort	Network;	// 网络MNC 2位
	uint	Network;	// 国家MCC + 网络MNC 3位+2位
	ushort	Area;		// 基站区域
	ushort	CellID;		// 基站编码

	GSM07();
	virtual ~GSM07();

	void Init(ITransport* port);
	void Init(COM idx, int baudrate = 115200);
	void Set(Pin power = P0, Pin rst = P0, Pin low = P0);

	virtual void Config();
	void SetLed(Pin led);
	void SetLed(OutputPort& led);
	void RemoveLed();

	virtual Socket* CreateSocket(NetType type);

	/******************************** 基础指令 ********************************/
	bool Test(int times = 10, int interval = 500);
	bool Reset(bool soft);
	String GetVersion();
	bool Sleep(uint ms);
	bool Echo(bool open);
	// 恢复出厂设置，将擦写所有保存到Flash的参数，恢复为默认参数。会导致模块重启
	bool Restore();
	String GetIMSI();
	String GetIMEI();
	// 查询SIM的CCID，也可以用于查询SIM是否存或者插好
	String GetCCID();

	/******************************** 网络服务 ********************************/
	// 获取运营商列表
	String GetMobiles();
	// 获取当前运营商
	String GetMobile();
	// 检查网络是否注册
	bool QueryRegister();
	String QuerySignal();

	bool AttachMT(bool enable);
	bool SetAPN(cstring apn, bool issgp = false);
	bool SetPDP(bool enable);
	IPAddress GetIP();
	bool SetClass(cstring mode);

	/******************************** TCP/IP ********************************/
	bool IPStart(const NetUri& remote);
	bool IPSend(int index, const Buffer& data);
	bool SendData(const String& cmd, const Buffer& bs);
	bool IPClose(int index);
	bool IPShutdown(int index);
	String IPStatus();
	bool SetAutoSendTimer(bool enable, ushort time);
	IPAddress DNSGetIP(const String& domain);
	bool IPMux(bool enable);
	bool IPHeartConfig(int index, int mode, int value);
	bool IPHeart(int index, bool enable);

	// 透明传输
	bool IPTransparentConfig(int mode, int value);
	bool IPTransparent(bool enable);

private:
	IPEndPoint	_Remote;	// 当前数据包远程地址

	// 打开与关闭
	virtual bool OnOpen();
	virtual void OnClose();
	// 检测连接
	virtual bool OnLink(uint retry);

	bool CheckReady();

	// 多个硬件socket
	int* _sockets[5];

	// 数据到达
	void OnReceive(Buffer& bs);
};

#endif
