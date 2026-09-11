#include "ui/LoginDialog.h"

#include "storage/AuthManager.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

LoginDialog::LoginDialog(AuthManager *auth, QWidget *parent)
    : QDialog(parent)
    , m_auth(auth)
{
    setWindowTitle(QStringLiteral("篮球联赛管理系统 - 登录"));
    setModal(true);
    resize(820, 540);

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    root->addWidget(buildBrandPanel());

    auto *right = new QWidget(this);
    auto *rv = new QVBoxLayout(right);
    rv->setContentsMargins(48, 40, 48, 40);

    m_stack = new QStackedWidget(right);
    m_stack->addWidget(buildLoginForm());
    m_stack->addWidget(buildRegisterForm());
    rv->addStretch();
    rv->addWidget(m_stack);
    rv->addStretch();
    root->addWidget(right, 1);

    m_loginUser->setFocus();
}

QWidget *LoginDialog::buildBrandPanel()
{
    auto *panel = new QFrame(this);
    panel->setFixedWidth(320);
    panel->setStyleSheet(QStringLiteral(
        "background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #C8102E, stop:1 #6E0A1C);"
        "border-top-left-radius:0px;"));

    auto *v = new QVBoxLayout(panel);
    v->setContentsMargins(34, 40, 34, 34);
    v->setSpacing(14);

    auto *logo = new QLabel(QStringLiteral("🏀"), panel);
    logo->setStyleSheet(QStringLiteral("font-size:54px; background:transparent;"));
    v->addWidget(logo);

    auto *title = new QLabel(QStringLiteral("篮球联赛\n管理系统"), panel);
    title->setStyleSheet(QStringLiteral("font-size:30px; font-weight:800; color:#FFFFFF; background:transparent;"));
    v->addWidget(title);

    auto *line = new QFrame(panel);
    line->setFixedHeight(3);
    line->setFixedWidth(60);
    line->setStyleSheet(QStringLiteral("background:#FDB927; border-radius:2px;"));
    v->addWidget(line);

    auto *desc = new QLabel(QStringLiteral("Basketball League\nManagement System"), panel);
    desc->setStyleSheet(QStringLiteral("color:#FFD9DE; font-size:13px; letter-spacing:1px; background:transparent;"));
    v->addWidget(desc);

    v->addStretch();

    auto *foot = new QLabel(QStringLiteral("C++ / Qt6 · 课程设计"), panel);
    foot->setStyleSheet(QStringLiteral("color:#FFB8C1; font-size:12px; background:transparent;"));
    v->addWidget(foot);
    return panel;
}

QWidget *LoginDialog::buildLoginForm()
{
    auto *page = new QWidget(this);
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("欢迎回来"), page);
    title->setObjectName(QStringLiteral("H1"));
    auto *sub = new QLabel(QStringLiteral("登录以管理场次、球员与数据"), page);
    sub->setObjectName(QStringLiteral("Muted"));
    v->addWidget(title);
    v->addWidget(sub);
    v->addSpacing(6);

    m_loginUser = new QLineEdit(page);
    m_loginUser->setPlaceholderText(QStringLiteral("用户名"));
    m_loginUser->setMinimumHeight(42);
    m_loginPass = new QLineEdit(page);
    m_loginPass->setPlaceholderText(QStringLiteral("密码"));
    m_loginPass->setEchoMode(QLineEdit::Password);
    m_loginPass->setMinimumHeight(42);

    m_loginError = new QLabel(page);
    m_loginError->setStyleSheet(QStringLiteral("color:#FF6B7A;"));
    m_loginError->setWordWrap(true);

    auto *loginBtn = new QPushButton(QStringLiteral("登 录"), page);
    loginBtn->setObjectName(QStringLiteral("Primary"));
    loginBtn->setMinimumHeight(44);

    auto *toReg = new QPushButton(QStringLiteral("还没有账号？立即注册"), page);
    toReg->setObjectName(QStringLiteral("Ghost"));
    toReg->setCursor(Qt::PointingHandCursor);

    v->addWidget(m_loginUser);
    v->addWidget(m_loginPass);
    v->addWidget(m_loginError);
    v->addWidget(loginBtn);
    v->addWidget(toReg);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::doLogin);
    connect(m_loginPass, &QLineEdit::returnPressed, this, &LoginDialog::doLogin);
    connect(m_loginUser, &QLineEdit::returnPressed, this, &LoginDialog::doLogin);
    connect(toReg, &QPushButton::clicked, this, [this]() {
        m_stack->setCurrentIndex(1);
        validateRegister();
    });
    return page;
}

QWidget *LoginDialog::buildRegisterForm()
{
    auto *page = new QWidget(this);
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("创建账号"), page);
    title->setObjectName(QStringLiteral("H1"));
    auto *sub = new QLabel(QStringLiteral("用户名至少 3 位；密码至少 8 位，且必须同时包含字母和数字"), page);
    sub->setObjectName(QStringLiteral("Muted"));
    sub->setWordWrap(true);
    v->addWidget(title);
    v->addWidget(sub);
    v->addSpacing(6);

    m_regUser = new QLineEdit(page);
    m_regUser->setPlaceholderText(QStringLiteral("用户名"));
    m_regUser->setMinimumHeight(42);
    m_regPass = new QLineEdit(page);
    m_regPass->setPlaceholderText(QStringLiteral("密码"));
    m_regPass->setEchoMode(QLineEdit::Password);
    m_regPass->setMinimumHeight(42);
    m_regPass2 = new QLineEdit(page);
    m_regPass2->setPlaceholderText(QStringLiteral("确认密码"));
    m_regPass2->setEchoMode(QLineEdit::Password);
    m_regPass2->setMinimumHeight(42);

    m_regError = new QLabel(page);
    m_regError->setStyleSheet(QStringLiteral("color:#FF6B7A;"));
    m_regError->setWordWrap(true);

    auto *regBtn = new QPushButton(QStringLiteral("注 册"), page);
    regBtn->setObjectName(QStringLiteral("Primary"));
    regBtn->setMinimumHeight(44);

    auto *toLogin = new QPushButton(QStringLiteral("已有账号？返回登录"), page);
    toLogin->setObjectName(QStringLiteral("Ghost"));
    toLogin->setCursor(Qt::PointingHandCursor);

    v->addWidget(m_regUser);
    v->addWidget(m_regPass);
    v->addWidget(m_regPass2);
    v->addWidget(m_regError);
    v->addWidget(regBtn);
    v->addWidget(toLogin);

    connect(regBtn, &QPushButton::clicked, this, &LoginDialog::doRegister);
    connect(m_regPass2, &QLineEdit::returnPressed, this, &LoginDialog::doRegister);
    // 输入过程中实时校验，逐项提示具体问题
    connect(m_regUser, &QLineEdit::textChanged, this, [this](const QString &) { validateRegister(); });
    connect(m_regPass, &QLineEdit::textChanged, this, [this](const QString &) { validateRegister(); });
    connect(m_regPass2, &QLineEdit::textChanged, this, [this](const QString &) { validateRegister(); });
    connect(toLogin, &QPushButton::clicked, this, [this]() {
        m_regError->clear();
        m_stack->setCurrentIndex(0);
    });
    return page;
}

void LoginDialog::doLogin()
{
    m_loginError->clear();
    QString error;
    if (m_auth->login(m_loginUser->text(), m_loginPass->text(), &error)) {
        m_username = m_loginUser->text().trimmed();
        accept();
    } else {
        m_loginError->setText(error);
    }
}

void LoginDialog::doRegister()
{
    if (!validateRegister()) {
        if (m_regError->text().isEmpty())
            m_regError->setText(QStringLiteral("请完整填写用户名和密码"));
        return;
    }
    QString error;
    if (m_auth->registerUser(m_regUser->text(), m_regPass->text(), &error)) {
        m_username = m_regUser->text().trimmed();
        accept();
    } else {
        m_regError->setStyleSheet(QStringLiteral("color:#FF6B7A;"));
        m_regError->setText(error);
    }
}

// 实时校验：边输入边给出具体的不合法原因；全部通过时显示绿色提示
bool LoginDialog::validateRegister()
{
    const QString user = m_regUser->text().trimmed();
    const QString pass = m_regPass->text();
    const QString pass2 = m_regPass2->text();

    QString error;
    if (!user.isEmpty() && user.size() < 3) {
        error = QStringLiteral("用户名至少 3 个字符");
    } else if (!pass.isEmpty()) {
        if (pass.size() < 8)
            error = QStringLiteral("密码至少 8 位");
        else if (pass.contains(QChar(' ')))
            error = QStringLiteral("密码不能包含空格");
        else {
            bool hasLetter = false;
            bool hasDigit = false;
            for (const QChar c : pass) {
                if (c.isLetter())
                    hasLetter = true;
                else if (c.isDigit())
                    hasDigit = true;
            }
            if (!hasLetter)
                error = QStringLiteral("密码必须包含字母");
            else if (!hasDigit)
                error = QStringLiteral("密码必须包含数字");
        }
    }
    if (error.isEmpty() && !pass.isEmpty() && !pass2.isEmpty() && pass2 != pass)
        error = QStringLiteral("两次输入的密码不一致");

    if (!error.isEmpty()) {
        m_regError->setStyleSheet(QStringLiteral("color:#FF6B7A;"));
        m_regError->setText(error);
        return false;
    }

    const bool complete = user.size() >= 3 && pass.size() >= 8 && pass == pass2;
    if (complete) {
        m_regError->setStyleSheet(QStringLiteral("color:#37D67A;"));
        m_regError->setText(QStringLiteral("✓ 校验通过，可以注册"));
        return true;
    }
    m_regError->clear();
    return false;
}
