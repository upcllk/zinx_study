#pragma once
#include <zinx.h>
#include "CmdVerify.h"

class EchoRole :
    public Irole
{
    // ͨ�� Irole �̳�
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;
};

class OutputCtrl :
    public Irole 
{
    // ͨ�� Irole �̳�
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;
    Ichannel* pOut = nullptr;
};

class DatePreRole :
    public Irole 
{
    // ͨ�� Irole �̳�
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;
    bool needAdd = false;
};