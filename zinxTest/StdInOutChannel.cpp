#include "StdInOutChannel.h"
#include <iostream>
#include "CmdVerify.h"

using namespace std;

bool StdInChannel::Init()
{
	return true;
}

bool StdInChannel::ReadFd(std::string& _input)
{
	cin >> _input;
	return true;
}

bool StdInChannel::WriteFd(std::string& _output)
{
	return false;
}

void StdInChannel::Fini()
{
}

int StdInChannel::GetFd()
{
	return STDIN_FILENO;
}

std::string StdInChannel::GetChannelInfo()
{
	return "stdin";
}

AZinxHandler* StdInChannel::GetInputNextStage(BytesMsg& _oInput)
{
	/*返回协议对象*/
	return CmdVerify::GetInstance();
}



bool StdOutChannel::Init()
{
	return true;
}

bool StdOutChannel::ReadFd(std::string& _input)
{
	return false;
}

bool StdOutChannel::WriteFd(std::string& _output)
{
	cout << _output << endl;
	return true;
}

void StdOutChannel::Fini()
{
}

int StdOutChannel::GetFd()
{
	return STDOUT_FILENO;
}

std::string StdOutChannel::GetChannelInfo()
{
	return "stdout";
}

AZinxHandler* StdOutChannel::GetInputNextStage(BytesMsg& _oInput)
{
	return nullptr;
}

AZinxHandler* MyTcpData::GetInputNextStage(BytesMsg& _oInput)
{
	/*返回协议对象*/
	return CmdVerify::GetInstance();
}

ZinxTcpData* MyFact::CreateTcpDataChannel(int _fd)
{
	return new MyTcpData(_fd);
}