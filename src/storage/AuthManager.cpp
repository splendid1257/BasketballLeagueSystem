#include "storage/AuthManager.h"

#include "storage/DataStore.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

AuthManager::AuthManager(DataStore *store)
    : m_store(store)
{
}

QString AuthManager::makeSalt()
{
    QByteArray salt(16, '\0');
    for (int i = 0; i < salt.size(); ++i)
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    return QString::fromLatin1(salt.toHex());
}

QString AuthManager::hashPassword(const QString &password, const QString &salt)
{
    const QByteArray raw = (salt + password).toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(raw, QCryptographicHash::Sha256).toHex());
}

bool AuthManager::registerUser(const QString &username, const QString &password, QString *error)
{
    const QString name = username.trimmed();
    if (name.isEmpty()) {
        if (error) *error = QStringLiteral("用户名不能为空");
        return false;
    }
    if (name.size() < 3) {
        if (error) *error = QStringLiteral("用户名至少 3 个字符");
        return false;
    }
    if (password.size() < 8) {
        if (error) *error = QStringLiteral("密码至少 8 位");
        return false;
    }
    if (password.contains(QChar(' '))) {
        if (error) *error = QStringLiteral("密码不能包含空格");
        return false;
    }
    bool hasLetter = false;
    bool hasDigit = false;
    for (const QChar c : password) {
        if (c.isLetter())
            hasLetter = true;
        else if (c.isDigit())
            hasDigit = true;
    }
    if (!hasLetter || !hasDigit) {
        if (error) *error = QStringLiteral("密码必须同时包含字母和数字");
        return false;
    }
    if (m_store->userExists(name)) {
        if (error) *error = QStringLiteral("该用户名已被注册");
        return false;
    }
    User u;
    u.username = name;
    u.salt = makeSalt();
    u.passwordHash = hashPassword(password, u.salt);
    m_store->addUser(u);
    return true;
}

bool AuthManager::login(const QString &username, const QString &password, QString *error)
{
    const QString name = username.trimmed();
    if (name.isEmpty() || password.isEmpty()) {
        if (error) *error = QStringLiteral("请输入用户名和密码");
        return false;
    }
    const User u = m_store->findUser(name);
    if (u.username.isEmpty()) {
        if (error) *error = QStringLiteral("用户不存在");
        return false;
    }
    if (hashPassword(password, u.salt) != u.passwordHash) {
        if (error) *error = QStringLiteral("密码错误");
        return false;
    }
    return true;
}
