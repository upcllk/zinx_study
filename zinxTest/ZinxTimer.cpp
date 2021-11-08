#include "ZinxTimer.h"
#include <sys/timerfd.h>
#include <unistd.h>

// ������ʱ���ļ�������, �ɹ� true ʧ�� false
bool ZinxTimerChannel::Init()
{
    int bRet = false;
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (iFd >= 0) {
        struct itimerspec period = {
            {1,0}, {1,0}
        };
        if (timerfd_settime(iFd, 0, &period, NULL) == 0) {
            bRet = true;
            m_Timerfd = iFd;
        }
    }
    return bRet;
}

// ��ȡ��ʱ������ һ����� 1 
bool ZinxTimerChannel::ReadFd(std::string& _input)
{
    bool bRet = false;
    char buf[8] = { 0 };
    if (read(m_Timerfd, buf, sizeof(buf)) == sizeof(buf)) {
        bRet = true;
        _input.assign(buf, sizeof(buf));
    }
    return bRet;
}

bool ZinxTimerChannel::WriteFd(std::string& _output)
{
    return false;
}

// �ر��ļ�������
void ZinxTimerChannel::Fini()
{

    close(m_Timerfd);
    m_Timerfd = -1;
}

// ���ص�ǰ�Ķ�ʱ�� fd
int ZinxTimerChannel::GetFd()
{
    return m_Timerfd;
}

std::string ZinxTimerChannel::GetChannelInfo()
{
    return "timerFd";
}

// ���ش���ʱʱ��Ķ���
AZinxHandler* ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput)
{
    // output_hello* pout_hello = new output_hello();
    // return pout_hello;
    return TimeOutMng::GetInstance();
}

IZinxMsg* output_hello::InternelHandle(IZinxMsg& _oInput)
{
    // ��ȡ���ͨ��
    auto pChannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
    std::string output = "hello world";
    ZinxKernel::Zinx_SendOut(output, *pChannel);
    return nullptr;
}

AZinxHandler* output_hello::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

TimeOutMng::TimeOutMng() {
    // ���� 10 ����
    for (int i = 0; i < WHELLSIZE; ++i) {
        std::list<TimerOutProc*> tmp;
        m_timer_whell.push_back(tmp);
    }
}

TimeOutMng* TimeOutMng::single = new TimeOutMng();

IZinxMsg* TimeOutMng::InternelHandle(IZinxMsg& _oInput)
{
    // ��ʱ���ٴ� ��ֹ��Ϊ������ɵĶ�γ�ʱ
    uint64_t iTimeOutCount = 0;
    GET_REF2DATA(BytesMsg, oBytes, _oInput);
    oBytes.szData.copy((char*)&iTimeOutCount, sizeof(iTimeOutCount), 0);

    while (iTimeOutCount-- > 0) {
        // �ƶ��̶�
        // ��ǰ�̶Ƚڵ�Ȧ�� -1 �� ִ�г�ʱ����
        m_cur_index = (m_cur_index + 1) % WHELLSIZE;
        std::list<TimerOutProc*> m_cache;
        for (auto ite = m_timer_whell[m_cur_index].begin();
            ite != m_timer_whell[m_cur_index].end(); ) {
            if ((*ite)->iCount <= 0) {
                // ���ڰ�ȫ����- 
                // Proc() : TimeOutMng::GetInstance()->DelTask(this);
                // ���Ը��ݻ����˼·����Ҫִ�г�ʱ������ task �浽һ����ʱ������ for ѭ����ͳһ����
                // (*ite)->Proc();
                m_cache.push_back(*ite);
                auto tmp = *ite;
                ite = m_timer_whell[m_cur_index].erase(ite);
                TimeOutMng::AddTask(tmp);
            }
            else {
                --(*ite)->iCount;
                ++ite;
            }
        }

        // ͳһ����ʱ����
        for (auto task : m_cache) {
            task->Proc();
        }
    }
    
    return nullptr;
}

AZinxHandler* TimeOutMng::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

void TimeOutMng::AddTask(TimerOutProc* _ptask)
{
    // ���㵱ǰ�������õ��ĸ����� �Լ� ����Ȧ��
    int index = (_ptask->GetTimeSec() + m_cur_index) % WHELLSIZE;
    _ptask->iCount = _ptask->GetTimeSec() / WHELLSIZE;
    m_timer_whell[index].push_back(_ptask);
    
    // _ptask->iCount = _ptask->GetTimeSec();
    // m_task_list.push_back(_ptask);
}

void TimeOutMng::DelTask(TimerOutProc* _ptask)
{
    // ����ʱ����ɾ������
    // Ĭ�Ͽ����ǵ� &
    // auto& -> list<TimerOutProc*>&
    for (auto& gear : m_timer_whell) {
        for (auto task : gear) {
            if (task == _ptask) {
                gear.remove(task);
                return;
            }
        }
    }
    // m_task_list.remove(_ptask);
}
