#include "CmdVerify.h"
#include "CmdMsg.h"
#include "Role.h"

CmdVerify* CmdVerify::poSingle = new CmdVerify();

CmdVerify::CmdVerify() {

}

CmdVerify::~CmdVerify() {

}

UserData* CmdVerify::raw2request(std::string _szInput)
{
    // ԭʼ����ת��Ϊ����ҵ������
    if (_szInput == "exit") {
        ZinxKernel::Zinx_Exit();
        return nullptr;
    }

    /*��ֵԭʼ�����ַ������û��������ַ����ֶ�*/
    auto pret = new CmdMsg();
    pret->szUserData = _szInput;

    if (_szInput == "open") {
        pret->isCmd = true;
        pret->isOpen = true;
    } 
    if (_szInput == "close") {
        pret->isCmd = true;
        pret->isOpen = false;
    }
    if (_szInput == "date") {
        pret->needDatePre = true;
        pret->isCmd = true;
    }
    if (_szInput == "cleardate") {
        pret->needDatePre = false;
        pret->isCmd = true;
    }

    return pret;
}

std::string* CmdVerify::response2raw(UserData& _oUserData)
{
    // ��̬����ת��
    GET_REF2DATA(CmdMsg, output, _oUserData);
    return new std::string(output.szUserData);
}

// ���զ����
Irole* CmdVerify::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    // ������Դ cin ���� tcp  
    szOutChannel = _oUserDataMsg.szInfo;
    if (szOutChannel == "stdin") {
        // cin ���յ�Ҫ�� cout ���
        szOutChannel = "stdout";
    }

    /*�������ͬ��������ͬ�Ĵ��� role ����*/
    auto roleList = ZinxKernel::Zinx_GetAllRole();

    auto pCmdMsg = dynamic_cast<CmdMsg*>(_oUserDataMsg.poUserData);
    /*��ȡ��ǰ��Ϣ�Ƿ�������*/
    bool isCmd = pCmdMsg->isCmd;
    Irole* pRetRole = nullptr;

    for (Irole* prole : roleList) {
        if (isCmd == true) {
            auto pOutCtrl = dynamic_cast<OutputCtrl*>(prole);
            if (pOutCtrl != nullptr) {
                // ��̬����ת���ɹ� --> ��ǰ prole ���� OutputCtrl* �������
                pRetRole = pOutCtrl;
                break;
            }
        }
        else {
            auto pDate = dynamic_cast<DatePreRole*>(prole);
            // ��̬����ת���ɹ� --> ��ǰ prole ���� EchoRole* �������
            if (pDate != nullptr) {
                pRetRole = pDate;
                break;
            }
        }
    }
    return pRetRole;
}

Ichannel* CmdVerify::GetMsgSender(BytesMsg& _oBytes)
{
    // ����ֱ��ȡ _oBytes.szInfo ��Ϊ _oBytes ����ԭʼ���ݶ����� Role �п���������γɵģ�
    // ��û�и� szInfo ��ֵ  --> ����������˼��
    return ZinxKernel::Zinx_GetChannel_ByInfo(szOutChannel);
}
