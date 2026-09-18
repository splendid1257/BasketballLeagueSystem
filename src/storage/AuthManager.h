#pragma once

#include <QString>

class DataStore;

// 密码加盐哈希存储
class AuthManager
{
public:
    explicit AuthManager(DataStore *store);

    // 成功返回 true，失败时写入 error 信息
    bool registerUser(const QString &username, const QString &password, QString *error);
    bool login(const QString &username, const QString &password, QString *error);

    // makeSalt 生成随机盐；hashPassword 按 salt+password 拼接后计算 SHA-256，拼接顺序不可颠倒
    static QString hashPassword(const QString &password, const QString &salt);
    static QString makeSalt();

private:
    DataStore *m_store;
};
