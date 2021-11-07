#pragma once
#include <zinx.h>

class CmdVerify :
    public Iprotocol
{
    // 通过 Iprotocol 继承
private:
    CmdVerify();
    virtual ~CmdVerify();
    static CmdVerify* poSingle;
public:
    virtual UserData* raw2request(std::string _szInput) override;
    virtual std::string* response2raw(UserData& _oUserData) override;
    virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;

    static CmdVerify* GetInstance() {
        return poSingle;
    }

    // 输出通道 -> cout 或者 tcp
    std::string szOutChannel;
};

