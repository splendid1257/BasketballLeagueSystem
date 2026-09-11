#pragma once

#include <QDialog>

class AuthManager;
class QLineEdit;
class QLabel;
class QStackedWidget;

// 登录 / 注册对话框
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(AuthManager *auth, QWidget *parent = nullptr);

    QString username() const { return m_username; }

private:
    QWidget *buildBrandPanel();
    QWidget *buildLoginForm();
    QWidget *buildRegisterForm();
    void doLogin();
    void doRegister();
    bool validateRegister();  // 实时校验注册表单；完全合法返回 true

    AuthManager *m_auth = nullptr;
    QStackedWidget *m_stack = nullptr;

    QLineEdit *m_loginUser = nullptr;
    QLineEdit *m_loginPass = nullptr;
    QLabel *m_loginError = nullptr;

    QLineEdit *m_regUser = nullptr;
    QLineEdit *m_regPass = nullptr;
    QLineEdit *m_regPass2 = nullptr;
    QLabel *m_regError = nullptr;

    QString m_username;
};
