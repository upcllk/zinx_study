#pragma once
#include <zinx.h>
#include <string>

class CmdMsg :
    public UserData
{
public:
    /*��ʾҪ���Ե��ַ���*/
    std::string szUserData;
    /*���������־*/
    bool isOpen = true;
    /*����Ϣ������*/
    bool isCmd = false;
    /*�Ƿ������ǰ׺*/
    bool needDatePre = false;
};

