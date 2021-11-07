#include "Role.h"
#include "CmdMsg.h"

bool EchoRole::Init()
{
	return true;
}

UserData* EchoRole::ProcMsg(UserData& _poUserData)
{
	/*��Ҫ��������һ��*/
	GET_REF2DATA(CmdMsg, input, _poUserData);
	CmdMsg* pout = new CmdMsg(input);
	// ����Ҳ�������� sendout ������ֱ����ҵ���ָ��ͨ���㷢�ͣ� ���ǲ�������ô��
	// ѡ��ͨ���������ý���Э���������������ò�������
	// ���ݴ�ͨ�������룬 ��Э���ַ��� ��ҵ��㴦����ͻ�Э���ʵ���� Э��㴦�����ķ�������
	ZinxKernel::Zinx_SendOut(*pout, *(CmdVerify::GetInstance()));
	return nullptr;
}

void EchoRole::Fini()
{
}

bool OutputCtrl::Init()
{
	/*������һ���������Ϊ Date ǰ׺����*/
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
	/*��ȡ���ر�־*/
	GET_REF2DATA(CmdMsg, input, _poUserData);
	if (input.isOpen == true) {
		// �����
		// �������� open ״̬�ٴ����� open �ᷢ���δ���
		if (pOut != nullptr) {
			ZinxKernel::Zinx_Add_Channel(*pOut);
			pOut = nullptr;
		}
	}
	else {
		// �����
		// ͬ������ close ״̬�ٴ����� close Ҳ�ᷢ���δ���
		auto pChannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		if (pChannel != nullptr) {
			ZinxKernel::Zinx_Del_Channel(*pChannel);
			// �ݴ棬�Ա��Ժ� open ֱ�ӿ���
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
	/*������һ���������Ϊ Echo ����*/
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
	/*date ���� / �������� --> ��Ӧ�ı��־λ*/
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
