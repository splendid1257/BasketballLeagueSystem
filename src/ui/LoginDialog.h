#pragma once

#include <QDialog>

class AuthManager;
class QLineEdit;
class QLabel;
class QStackedWidget;

// 登录/注册入口对话框：驱动 AuthManager 完成认证，失败信息在表单内联展示
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
    // 索引 0=登录页、1=注册页，两表单共用同一容器互切
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
