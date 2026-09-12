#pragma once

#include <QString>

struct User
{
    QString username;
    QString salt;
    QString passwordHash;  // SHA-256(salt + password) 的十六进制
};
