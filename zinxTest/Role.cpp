#include "Role.h"
#include "CmdMsg.h"

bool EchoRole::Init()
{
	return true;
}

UserData* EchoRole::ProcMsg(UserData& _poUserData)
{
	/*需要拷贝构造一下*/
	GET_REF2DATA(CmdMsg, input, _poUserData);
	CmdMsg* pout = new CmdMsg(input);
	// 这里也可以利用 sendout 的重载直接由业务层指定通道层发送， 但是不建议这么做
	// 选择通道这件事最好交个协议层做，避免跨层调用产生依赖
	// 数据从通道层输入， 经协议层分发， 到业务层处理后发送回协议层实例， 协议层处理往哪发的问题
	ZinxKernel::Zinx_SendOut(*pout, *(CmdVerify::GetInstance()));
	return nullptr;
}

void EchoRole::Fini()
{
}

bool OutputCtrl::Init()
{
	/*设置下一个处理对象为 Date 前缀对象*/
	Irole* pRetRole = nullptr;
	for (auto pRole : ZinxKernel::Zinx_GetAllRole()) {
		auto pDate = dynamic_cast<DatePreRole*>(pRole);
		if (pDate != nullptr) {
			pRetRole = pDate;
			break;
		}
	}
	if (pRetRole != nullptr) {
		SetNextProcessor(*pRetRole);
	}
	return true;
}

UserData* OutputCtrl::ProcMsg(UserData& _poUserData)
{
	/*读取开关标志*/
	GET_REF2DATA(CmdMsg, input, _poUserData);
	if (input.isOpen == true) {
		// 开输出
		// 但是这里 open 状态再次输入 open 会发生段错误
		if (pOut != nullptr) {
			ZinxKernel::Zinx_Add_Channel(*pOut);
			pOut = nullptr;
		}
	}
	else {
		// 关输出
		// 同样这里 close 状态再次输入 close 也会发生段错误
		auto pChannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		if (pChannel != nullptr) {
			ZinxKernel::Zinx_Del_Channel(*pChannel);
			// 暂存，以便以后 open 直接开启
			pOut = pChannel;
		}
	}
	return new CmdMsg(input);
}

void OutputCtrl::Fini()
{
}

bool DatePreRole::Init()
{
	/*设置下一个处理对象为 Echo 对象*/
	Irole* pRetRole = nullptr;
	for (auto pRole : ZinxKernel::Zinx_GetAllRole()) {
		auto pEcho = dynamic_cast<EchoRole*>(pRole);
		if (pEcho != nullptr) {
			pRetRole = pEcho;
			break;
		}
	}
	if (pRetRole != nullptr) {
		SetNextProcessor(*pRetRole);
	}
	return true;
}

UserData* DatePreRole::ProcMsg(UserData& _poUserData)
{
	/*date 命令 / 回显数据 --> 对应改变标志位*/
	CmdMsg* pret = nullptr;
	GET_REF2DATA(CmdMsg, input, _poUserData);
	if (input.isCmd) {
		needAdd = input.needDatePre;
	}
	else {
		if (needAdd == true) {
			time_t tmp;
			time(&tmp);
			std::string szNew = std::string(ctime(&tmp)) + input.szUserData;
			pret = new CmdMsg(input);
			pret->szUserData = szNew;
		}
		else {
			pret = new CmdMsg(input);
		}
	}
	return pret;
}

void DatePreRole::Fini()
{
}
