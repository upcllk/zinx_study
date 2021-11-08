#include <zinx.h>
#include "ZinxTimer.h"

class TimerHello :
	public TimerOutProc
{
public:
	TimerHello(int _freqSenconds);
private:
	int m_freqSeconds;
	// 通过 TimerOutProc 继承
	virtual void Proc() override
	{
		auto pChannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		std::string output = "hello world";
		ZinxKernel::Zinx_SendOut(output, *pChannel);
	}
	virtual int GetTimeSec() override
	{
		return m_freqSeconds;
	}
};

class TimerBye :
	public TimerOutProc
{
public:
	TimerBye(int _freqSenconds);
private:
	int m_freqSeconds;
	// 通过 TimerOutProc 继承
	virtual void Proc() override   
	{
		auto pChannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		std::string output = "bye bye";
		ZinxKernel::Zinx_SendOut(output, *pChannel);
	}
	virtual int GetTimeSec() override
	{
		return m_freqSeconds;
	}
};