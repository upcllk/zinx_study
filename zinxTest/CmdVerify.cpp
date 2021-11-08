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
    // 原始数据转换为请求业务数据
    if (_szInput == "exit") {
        ZinxKernel::Zinx_Exit();
        return nullptr;
    }

    /*赋值原始数据字符串到用户数据中字符串字段*/
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
    // 动态类型转换
    GET_REF2DATA(CmdMsg, output, _oUserData);
    return new std::string(output.szUserData);
}

// 这个咋来的
Irole* CmdVerify::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    // 数据来源 cin 或者 tcp  
    szOutChannel = _oUserDataMsg.szInfo;
    if (szOutChannel == "stdin") {
        // cin 接收的要用 cout 输出
        szOutChannel = "stdout";
    }

    /*根据命令不同，交给不同的处理 role 对象*/
    auto roleList = ZinxKernel::Zinx_GetAllRole();

    auto pCmdMsg = dynamic_cast<CmdMsg*>(_oUserDataMsg.poUserData);
    /*读取当前消息是否是命令*/
    bool isCmd = pCmdMsg->isCmd;
    Irole* pRetRole = nullptr;

    for (Irole* prole : roleList) {
        if (isCmd == true) {
            auto pOutCtrl = dynamic_cast<OutputCtrl*>(prole);
            if (pOutCtrl != nullptr) {
                // 动态类型转换成功 --> 当前 prole 就是 OutputCtrl* 子类对象
                pRetRole = pOutCtrl;
                break;
            }
        }
        else {
            auto pDate = dynamic_cast<DatePreRole*>(prole);
            // 动态类型转换成功 --> 当前 prole 就是 EchoRole* 子类对象
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
    // 不能直接取 _oBytes.szInfo 因为 _oBytes 不是原始数据而是在 Role 中拷贝构造后形成的，
    // 并没有给 szInfo 赋值  --> 坏了是这意思不
    return ZinxKernel::Zinx_GetChannel_ByInfo(szOutChannel);
}
