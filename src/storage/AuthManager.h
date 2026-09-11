#pragma once

#include <QString>

class DataStore;

// 账号管理：注册 / 登录，密码加盐哈希存储
class AuthManager
{
public:
    explicit AuthManager(DataStore *store);

    // 成功返回 true；失败时写入 error 文案
    bool registerUser(const QString &username, const QString &password, QString *error);
    bool login(const QString &username, const QString &password, QString *error);

    static QString hashPassword(const QString &password, const QString &salt);
    static QString makeSalt();

private:
    DataStore *m_store;
};
