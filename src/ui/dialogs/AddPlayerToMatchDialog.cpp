#include "ui/dialogs/AddPlayerToMatchDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

AddPlayerToMatchDialog::AddPlayerToMatchDialog(const QVector<Player> &candidates,
                                               const QString &team1Name,
                                               const QString &team2Name,
                                               int presetTeam,
                                               QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("添加参赛队员"));
    setModal(true);
    setMinimumWidth(420);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 18);
    root->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("添加参赛队员"), this);
    title->setObjectName(QStringLiteral("H2"));
    root->addWidget(title);

    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_team = new QComboBox(this);
    m_team->addItem(team1Name, 1);
    m_team->addItem(team2Name, 2);
    m_team->setCurrentIndex(presetTeam == 2 ? 1 : 0);

    m_player = new QComboBox(this);
    for (const Player &p : candidates) {
        const QString label = QStringLiteral("%1（%2）· %3")
                                  .arg(p.name, p.id, p.team.isEmpty() ? QStringLiteral("无球队") : p.team);
        m_player->addItem(label, p.id);
    }
    m_player->setEnabled(!candidates.isEmpty());

    m_tp = new QSpinBox(this);
    m_rb = new QSpinBox(this);
    m_dk = new QSpinBox(this);
    m_st = new QSpinBox(this);
    for (QSpinBox *sb : {m_tp, m_rb, m_dk, m_st})
        sb->setRange(0, 200);

    form->addRow(QStringLiteral("加入球队"), m_team);
    form->addRow(QStringLiteral("选择球员"), m_player);
    form->addRow(QStringLiteral("三分球个数"), m_tp);
    form->addRow(QStringLiteral("篮板球个数"), m_rb);
    form->addRow(QStringLiteral("扣篮成功次数"), m_dk);
    form->addRow(QStringLiteral("抢断次数"), m_st);
    root->addLayout(form);
    root->addStretch();

    if (candidates.isEmpty()) {
        auto *hint = new QLabel(QStringLiteral("没有可添加的球员：请先在“球员管理”中创建，或该场次已包含全部球员。"),
                                this);
        hint->setObjectName(QStringLiteral("Muted"));
        hint->setWordWrap(true);
        root->addWidget(hint);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setObjectName(QStringLiteral("Primary"));
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("添加"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    connect(buttons, &QDialogButtonBox::accepted, this, &AddPlayerToMatchDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &AddPlayerToMatchDialog::reject);
    root->addWidget(buttons);
}

int AddPlayerToMatchDialog::teamNo() const
{
    return m_team->currentData().toInt();
}

PlayerStats AddPlayerToMatchDialog::stats() const
{
    PlayerStats s;
    s.playerId = m_player->currentData().toString();
    s.playerName = m_player->currentText().section(QStringLiteral("（"), 0, 0).trimmed();
    s.threePointers = m_tp->value();
    s.rebounds = m_rb->value();
    s.dunks = m_dk->value();
    s.steals = m_st->value();
    return s;
}

void AddPlayerToMatchDialog::accept()
{
    if (m_player->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请选择要添加的球员"));
        return;
    }
    QDialog::accept();
}
