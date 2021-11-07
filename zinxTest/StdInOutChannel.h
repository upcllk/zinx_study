#pragma once
#include <zinx.h>
#include <ZinxTCP.h>

class StdInChannel :
    public Ichannel
{
    // 通过 Ichannel 继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class StdOutChannel :
    public Ichannel {
    // 通过 Ichannel 继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

/*TCP数据通道类*/
class MyTcpData
    :public ZinxTcpData 
{
public:
    MyTcpData(int _fd) :ZinxTcpData(_fd) {}
    // 通过 ZinxTcpData 继承
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

/*TCP连接工厂类， 构造TCP数据通道类*/
class MyFact
    :public IZinxTcpConnFact
{
    // 通过 IZinxTcpConnFact 继承
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};