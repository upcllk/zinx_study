#pragma once
#include <zinx.h>
#include <string>

class CmdMsg :
    public UserData
{
public:
    /*表示要回显的字符串*/
    std::string szUserData;
    /*开启输出标志*/
    bool isOpen = true;
    /*该消息是命令*/
    bool isCmd = false;
    /*是否加日期前缀*/
    bool needDatePre = false;
};

