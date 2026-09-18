#include "ui/dialogs/TeamEditDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

// 新建与编辑共用：创建时字段留空，编辑时由 setTeam 预填
TeamEditDialog::TeamEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("球队信息"));
    setModal(true);
    setMinimumWidth(420);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 18);
    root->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("新增球队"), this);
    title->setObjectName(QStringLiteral("H2"));
    m_titleLabel = title;
    root->addWidget(title);

    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_name = new QLineEdit(this);
    m_name->setPlaceholderText(QStringLiteral("例如 洛杉矶湖人"));
    m_city = new QLineEdit(this);
    m_city->setPlaceholderText(QStringLiteral("例如 洛杉矶"));
    m_coach = new QLineEdit(this);
    m_coach->setPlaceholderText(QStringLiteral("主教练姓名"));
    m_arena = new QLineEdit(this);
    m_arena->setPlaceholderText(QStringLiteral("例如 斯台普斯中心"));

    form->addRow(QStringLiteral("球队名称"), m_name);
    form->addRow(QStringLiteral("所在城市"), m_city);
    form->addRow(QStringLiteral("主教练"), m_coach);
    form->addRow(QStringLiteral("主场球馆"), m_arena);
    root->addLayout(form);
    root->addStretch();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setObjectName(QStringLiteral("Primary"));
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    connect(buttons, &QDialogButtonBox::accepted, this, &TeamEditDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &TeamEditDialog::reject);
    root->addWidget(buttons);
}

// 预填已有球队字段并切换标题为“编辑球队”
void TeamEditDialog::setTeam(const Team &t)
{
    m_name->setText(t.name);
    m_city->setText(t.city);
    m_coach->setText(t.coach);
    m_arena->setText(t.arena);
    if (m_titleLabel)
        m_titleLabel->setText(QStringLiteral("编辑球队"));
}

// 从控件拼装 Team；仅名称必填，其余可为空
Team TeamEditDialog::team() const
{
    Team t;
    t.name = m_name->text().trimmed();
    t.city = m_city->text().trimmed();
    t.coach = m_coach->text().trimmed();
    t.arena = m_arena->text().trimmed();
    return t;
}

// 名称必填，作为球队主键不可为空
void TeamEditDialog::accept()
{
    if (m_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写球队名称"));
        return;
    }
    QDialog::accept();
}
