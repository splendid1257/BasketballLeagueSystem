#pragma once

#include <QString>

// 系统用户（登录/注册）
struct User
{
    QString username;
    QString salt;          // 随机盐
    QString passwordHash;  // SHA-256(salt + password) 的十六进制
};
