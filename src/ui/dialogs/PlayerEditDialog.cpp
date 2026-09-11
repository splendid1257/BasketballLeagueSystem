#include "ui/dialogs/PlayerEditDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

PlayerEditDialog::PlayerEditDialog(const QStringList &teams, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("球员信息"));
    setModal(true);
    setMinimumWidth(420);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 18);
    root->setSpacing(14);

    auto *title = new QLabel(this);
    title->setObjectName(QStringLiteral("H2"));
    title->setText(QStringLiteral("新增球员"));
    m_titleLabel = title;
    root->addWidget(title);

    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_id = new QLineEdit(this);
    m_id->setPlaceholderText(QStringLiteral("例如 L001"));
    m_name = new QLineEdit(this);
    m_name->setPlaceholderText(QStringLiteral("球员姓名"));
    m_age = new QSpinBox(this);
    m_age->setRange(10, 80);
    m_age->setValue(20);
    m_team = new QComboBox(this);
    m_team->setEditable(true);
    m_team->addItems(teams);

    form->addRow(QStringLiteral("编号"), m_id);
    form->addRow(QStringLiteral("姓名"), m_name);
    form->addRow(QStringLiteral("年龄"), m_age);
    form->addRow(QStringLiteral("所属球队"), m_team);
    root->addLayout(form);
    root->addStretch();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setObjectName(QStringLiteral("Primary"));
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    connect(buttons, &QDialogButtonBox::accepted, this, &PlayerEditDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &PlayerEditDialog::reject);
    root->addWidget(buttons);
}

void PlayerEditDialog::setPlayer(const Player &p)
{
    m_editMode = true;
    m_id->setText(p.id);
    m_name->setText(p.name);
    m_age->setValue(p.age);
    const int idx = m_team->findText(p.team);
    if (idx >= 0)
        m_team->setCurrentIndex(idx);
    else
        m_team->setEditText(p.team);
    if (m_titleLabel)
        m_titleLabel->setText(QStringLiteral("编辑球员"));
}

Player PlayerEditDialog::player() const
{
    Player p;
    p.id = m_id->text().trimmed();
    p.name = m_name->text().trimmed();
    p.age = m_age->value();
    p.team = m_team->currentText().trimmed();
    return p;
}

void PlayerEditDialog::accept()
{
    if (m_id->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写球员编号"));
        return;
    }
    if (m_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写球员姓名"));
        return;
    }
    QDialog::accept();
}
