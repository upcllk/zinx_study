#include <zinx.h>
#include <iostream>

using namespace std;

/* Echo */
/*2��д���ܴ�����*/
class Echo :public AZinxHandler {
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		// ��Ϣ�������� ʵ�� Echo ����
		// ����һ�� ByteMsg ���͵� input ָ�� _oInput ? 
		GET_REF2DATA(BytesMsg, input, _oInput);
		cout << input.szData << endl;
		return nullptr;
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return nullptr;
	}
}*poEcho = new Echo();
// �������ʱ��˳���ٶ��������¶���

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
		return poEcho;
	}
};

int main() {
	/*1����ʼ�����*/
	ZinxKernel::ZinxKernelInit();

	/*4����ͨ��������ӵ����*/
	TestStdin* poStdin = new TestStdin();
	ZinxKernel::Zinx_Add_Channel(*poStdin);

	ZinxKernel::Zinx_Run();
	ZinxKernel::ZinxKernelFini();
	return 0;
}