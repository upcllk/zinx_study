#include <zinx.h>
#include <iostream>
#include "StdInOutChannel.h"
#include "Role.h"
#include <ZinxTCP.h>

using namespace std;

/* 
Echo
	输入 exit 时不做回显 退出程序
	输入 close 不做回显， 直至输入 open 之后才做回显， 类似于动态开关
	输入 date 打开日期前缀功能	cleardate 关闭
*/

/*创建标准输出通道类*/
class TestStdout :public Ichannel {
	// 通过 Ichannel 继承
	virtual bool Init() override
	{
		return true;
	}
	virtual bool ReadFd(std::string& _input) override
	{
		return false;
	}
	virtual bool WriteFd(std::string& _output) override
	{
		cout << _output << endl;
		return true;
	}
	virtual void Fini() override
	{
	}
	virtual int GetFd() override
	{
		return STDOUT_FILENO;
	}
	virtual std::string GetChannelInfo() override
	{
		return "stdout";
	}
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return nullptr;
	}
}*poStdout = new TestStdout();

/*2、写功能处理类*/
class Echo :public AZinxHandler {
	// 通过 AZinxHandler 继承
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		// 信息处理函数， 实现 Echo 功能
		GET_REF2DATA(BytesMsg, input, _oInput);

		/*判断是否退出, 但是如果在这里直接 zinx_exit 退出会违反开闭原则， 不利于添加新功能
			--> 创建退出程序类先过滤
		*/
		ZinxKernel::Zinx_SendOut(input.szData, *poStdout);
		return nullptr;
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return nullptr;
	}
}*poEcho = new Echo();

class ExitFramework :public AZinxHandler {
	// 通过 AZinxHandler 继承
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, oByte, _oInput);
		if (oByte.szData == "exit") {
			ZinxKernel::Zinx_Exit();
			return nullptr;
		}
		/*创建交给下一个处理环节的数据*/
		return new BytesMsg(oByte);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}
}*poExit = new ExitFramework();

class AddDate :public AZinxHandler {
	// 通过 AZinxHandler 继承
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oInput);
		time_t tmp;
		time(&tmp);
		string szNew = string(ctime(&tmp)) + oBytes.szData;
		// 拷贝构造
		BytesMsg* pret = new BytesMsg(oBytes);
		pret->szData = szNew;
		return pret;
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}
}*poAddDate = new AddDate();

class CmdHandler :public AZinxHandler {
	// 通过 AZinxHandler 继承
	int status = 0;	// 0 - 不加前缀， 1 - 加前缀
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		/*判断输入是否是 open 或者 close*/
		GET_REF2DATA(BytesMsg, oBytes, _oInput);
		if (oBytes.szData == "open") {
			ZinxKernel::Zinx_Add_Channel(*poStdout);
			return nullptr;
		}
		else if (oBytes.szData == "close") {
			ZinxKernel::Zinx_Del_Channel(*poStdout);
			return nullptr;
		}
		else if (oBytes.szData == "date") {
			status = 1;
		}
		else if (oBytes.szData == "cleardate") {
			status = 0;
		}
		// 这个对象会传递到 GetNextHandler 中处理
		return new BytesMsg(oBytes);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		/*根据不同消息选择不同处理者*/
		GET_REF2DATA(BytesMsg, oBytes, _oNextMsg);
		if (oBytes.szData == "exit") {
			return poExit;
		}
		else {
			if (status == 0) {
				return poEcho;
			}
			else if (status == 1) {
				return poAddDate;
			}
			return poEcho;
		}

		return nullptr;
	}
}*poCmd = new CmdHandler();

/*3、写通道类*/
class TestStdin : public Ichannel {
	// 通过 Ichannel 继承
	virtual bool Init() override
	{
		return true;
	}
	virtual bool ReadFd(std::string& _input) override
	{
		cin >> _input;
		return true;
	}
	virtual bool WriteFd(std::string& _output) override
	{ 
		return false;
	}
	virtual void Fini() override
	{
	}
	virtual int GetFd() override
	{
		return STDIN_FILENO;
	}
	virtual std::string GetChannelInfo() override
	{
		return "stdin";
	}
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return poCmd;
	}
};


int main() {
	/*1、初始化框架*/
	ZinxKernel::ZinxKernelInit();

	/*4、将通道对象添加到框架*/
	/*顺序影响吗  好像会影响*/
	ZinxKernel::Zinx_Add_Channel(*(new StdInChannel()));
	ZinxKernel::Zinx_Add_Channel(*(new StdOutChannel()));
	// 创建 TCP 监听类加入到 Kernel	指定端口号和工厂对象
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(51111, new MyFact())));
	ZinxKernel::Zinx_Add_Role(*(new EchoRole()));
	ZinxKernel::Zinx_Add_Role(*(new DatePreRole()));
	ZinxKernel::Zinx_Add_Role(*(new OutputCtrl()));
	/*
	TestStdin* poStdin = new TestStdin();
	ZinxKernel::Zinx_Add_Channel(*poStdin);

	// 这里注释掉的话就必须先输入 open 才能打开输出功能
	ZinxKernel::Zinx_Add_Channel(*poStdout);
	*/

	ZinxKernel::Zinx_Run();

	ZinxKernel::ZinxKernelFini();
	return 0;
}	