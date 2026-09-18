#pragma once

#include <QString>

// 认证凭据：仅存 salt 与哈希，绝不落地明文密码
struct User
{
    QString username;
    QString salt;
    QString passwordHash;  // SHA-256(salt + password) 的十六进制
};
