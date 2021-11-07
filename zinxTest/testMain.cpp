#include <zinx.h>
#include <iostream>
#include "StdInOutChannel.h"
#include "Role.h"
#include <ZinxTCP.h>

using namespace std;

/* 
Echo
	���� exit ʱ�������� �˳�����
	���� close �������ԣ� ֱ������ open ֮��������ԣ� �����ڶ�̬����
	���� date ������ǰ׺����	cleardate �ر�
*/

/*������׼���ͨ����*/
class TestStdout :public Ichannel {
	// ͨ�� Ichannel �̳�
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

/*2��д���ܴ�����*/
class Echo :public AZinxHandler {
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		// ��Ϣ�������� ʵ�� Echo ����
		GET_REF2DATA(BytesMsg, input, _oInput);

		/*�ж��Ƿ��˳�, �������������ֱ�� zinx_exit �˳���Υ������ԭ�� ����������¹���
			--> �����˳��������ȹ���
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
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, oByte, _oInput);
		if (oByte.szData == "exit") {
			ZinxKernel::Zinx_Exit();
			return nullptr;
		}
		/*����������һ�������ڵ�����*/
		return new BytesMsg(oByte);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}
}*poExit = new ExitFramework();

class AddDate :public AZinxHandler {
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oInput);
		time_t tmp;
		time(&tmp);
		string szNew = string(ctime(&tmp)) + oBytes.szData;
		// ��������
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
	// ͨ�� AZinxHandler �̳�
	int status = 0;	// 0 - ����ǰ׺�� 1 - ��ǰ׺
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		/*�ж������Ƿ��� open ���� close*/
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
		// �������ᴫ�ݵ� GetNextHandler �д���
		return new BytesMsg(oBytes);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		/*���ݲ�ͬ��Ϣѡ��ͬ������*/
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

/*3��дͨ����*/
class TestStdin : public Ichannel {
	// ͨ�� Ichannel �̳�
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
	/*1����ʼ�����*/
	ZinxKernel::ZinxKernelInit();

	/*4����ͨ��������ӵ����*/
	/*˳��Ӱ����  �����Ӱ��*/
	ZinxKernel::Zinx_Add_Channel(*(new StdInChannel()));
	ZinxKernel::Zinx_Add_Channel(*(new StdOutChannel()));
	// ���� TCP ��������뵽 Kernel	ָ���˿ںź͹�������
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(51111, new MyFact())));
	ZinxKernel::Zinx_Add_Role(*(new EchoRole()));
	ZinxKernel::Zinx_Add_Role(*(new DatePreRole()));
	ZinxKernel::Zinx_Add_Role(*(new OutputCtrl()));
	/*
	TestStdin* poStdin = new TestStdin();
	ZinxKernel::Zinx_Add_Channel(*poStdin);

	// ����ע�͵��Ļ��ͱ��������� open ���ܴ��������
	ZinxKernel::Zinx_Add_Channel(*poStdout);
	*/

	ZinxKernel::Zinx_Run();

	ZinxKernel::ZinxKernelFini();
	return 0;
}	