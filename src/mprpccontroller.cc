#include "mprpccontroller.h"

MprpcController::MprpcController()//初始化，一开始认为是正确的
{
    m_failed = false;
    m_errText = "";
}

void MprpcController::Reset()//重置
{
    m_failed = false;
    m_errText = "";
}

bool MprpcController::Failed() const//判断当前调用的成功与否
{
    return m_failed;
}

std::string MprpcController::ErrorText() const//错误信息
{
    return m_errText;
}

void MprpcController::SetFailed(const std::string& reason)//设置错误
{
    m_failed = true;
    m_errText = reason;
}

//不需要
void MprpcController::StartCancel(){}
bool MprpcController::IsCanceled() const {return false;}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}