#include "ui/dialogs/MatchDetailDialog.h"

#include "model/Match.h"
#include "storage/DataStore.h"
#include "ui/dialogs/AddPlayerToMatchDialog.h"
#include "ui/dialogs/MatchEditDialog.h"
#include "ui/widgets/UiUtils.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {

QTableWidget *makeStatsTable(QWidget *parent)
{
    auto *table = new QTableWidget(parent);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({QStringLiteral("编号"), QStringLiteral("姓名"),
                                      QStringLiteral("三分"), QStringLiteral("篮板"),
                                      QStringLiteral("扣篮"), QStringLiteral("抢断"),
                                      QStringLiteral("得分")});
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    for (int c = 2; c < 7; ++c)
        table->horizontalHeader()->setSectionResizeMode(c, QHeaderView::ResizeToContents);
    return table;
}

void fillStatsTable(QTableWidget *table, const QVector<PlayerStats> &list)
{
    ui::beginTableFill(table);
    table->setRowCount(list.size());
    for (int r = 0; r < list.size(); ++r) {
        const PlayerStats &s = list.at(r);
        auto set = [&](int c, const QString &text, int align = Qt::AlignCenter) {
            auto *item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::Alignment(align) | Qt::AlignVCenter);
            table->setItem(r, c, item);
        };
        set(0, s.playerId);
        set(1, s.playerName, Qt::AlignLeft);
        set(2, QString::number(s.threePointers));
        set(3, QString::number(s.rebounds));
        set(4, QString::number(s.dunks));
        set(5, QString::number(s.steals));
        auto *pts = new QTableWidgetItem(QString::number(s.points()));
        pts->setTextAlignment(Qt::AlignCenter);
        pts->setForeground(QColor(QStringLiteral("#FDB927")));
        table->setItem(r, 6, pts);
    }
    ui::endTableFill(table, {1});
}

// 自由函数：一次性对话框，无需成员与信号
bool promptStats(QWidget *parent, const QString &who, PlayerStats &s)
{
    QDialog dlg(parent);
    dlg.setWindowTitle(QStringLiteral("编辑数据"));
    dlg.setMinimumWidth(360);

    auto *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(20, 18, 20, 16);
    root->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("编辑 %1 的本场数据").arg(who), &dlg);
    title->setObjectName(QStringLiteral("H3"));
    root->addWidget(title);

    auto *form = new QFormLayout();
    form->setSpacing(10);
    auto *tp = new QSpinBox(&dlg);
    auto *rb = new QSpinBox(&dlg);
    auto *dk = new QSpinBox(&dlg);
    auto *st = new QSpinBox(&dlg);
    for (QSpinBox *sb : {tp, rb, dk, st})
        sb->setRange(0, 200);
    tp->setValue(s.threePointers);
    rb->setValue(s.rebounds);
    dk->setValue(s.dunks);
    st->setValue(s.steals);
    form->addRow(QStringLiteral("三分球个数"), tp);
    form->addRow(QStringLiteral("篮板球个数"), rb);
    form->addRow(QStringLiteral("扣篮成功次数"), dk);
    form->addRow(QStringLiteral("抢断次数"), st);
    root->addLayout(form);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    buttons->button(QDialogButtonBox::Ok)->setObjectName(QStringLiteral("Primary"));
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    root->addWidget(buttons);

    if (dlg.exec() != QDialog::Accepted)
        return false;
    s.threePointers = tp->value();
    s.rebounds = rb->value();
    s.dunks = dk->value();
    s.steals = st->value();
    return true;
}

}  // namespace

MatchDetailDialog::MatchDetailDialog(DataStore *store, const QString &matchId, QWidget *parent)
    : QDialog(parent)
    , m_store(store)
    , m_matchId(matchId)
{
    setWindowTitle(QStringLiteral("场次详情"));
    setModal(true);
    resize(1000, 640);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 18);
    root->setSpacing(16);

    auto *header = new QFrame(this);
    header->setObjectName(QStringLiteral("Card"));
    auto *hl = new QVBoxLayout(header);
    hl->setContentsMargins(20, 16, 20, 16);
    hl->setSpacing(4);

    m_title = new QLabel(header);
    m_title->setObjectName(QStringLiteral("H2"));
    m_score = new QLabel(header);
    m_score->setObjectName(QStringLiteral("CardValue"));
    m_meta = new QLabel(header);
    m_meta->setObjectName(QStringLiteral("Muted"));

    auto *editInfoBtn = new QPushButton(QStringLiteral("编辑场次信息"), header);
    auto *headRow = new QHBoxLayout();
    headRow->addWidget(m_title);
    headRow->addStretch();
    headRow->addWidget(editInfoBtn);
    hl->addLayout(headRow);
    hl->addWidget(m_score);
    hl->addWidget(m_meta);
    connect(editInfoBtn, &QPushButton::clicked, this, &MatchDetailDialog::editMatchInfo);
    root->addWidget(header);

    auto *panels = new QHBoxLayout();
    panels->setSpacing(16);
    panels->addWidget(buildTeamPanel(1, m_table1), 1);
    panels->addWidget(buildTeamPanel(2, m_table2), 1);
    root->addLayout(panels, 1);

    auto *closeBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    closeBox->button(QDialogButtonBox::Close)->setText(QStringLiteral("关闭"));
    connect(closeBox, &QDialogButtonBox::rejected, this, &QDialog::accept);
    root->addWidget(closeBox);

    connect(m_store, &DataStore::changed, this, &MatchDetailDialog::reload);
    reload();
}

QWidget *MatchDetailDialog::buildTeamPanel(int teamNo, QTableWidget *&table)
{
    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("Card"));
    auto *v = new QVBoxLayout(card);
    v->setContentsMargins(16, 14, 16, 14);
    v->setSpacing(10);

    auto *head = new QHBoxLayout();
    auto *name = new QLabel(card);
    name->setObjectName(QStringLiteral("H3"));
    if (teamNo == 1)
        m_team1Label = name;
    else
        m_team2Label = name;
    head->addWidget(name);
    head->addStretch();

    auto *addBtn = new QPushButton(QStringLiteral("＋ 添加球员"), card);
    addBtn->setObjectName(QStringLiteral("Primary"));
    auto *delBtn = new QPushButton(QStringLiteral("移除"), card);
    delBtn->setObjectName(QStringLiteral("Danger"));
    auto *editBtn = new QPushButton(QStringLiteral("编辑数据"), card);
    head->addWidget(editBtn);
    head->addWidget(delBtn);
    head->addWidget(addBtn);
    v->addLayout(head);

    table = makeStatsTable(card);
    v->addWidget(table, 1);

    connect(addBtn, &QPushButton::clicked, this, [this, teamNo]() { addPlayer(teamNo); });
    connect(delBtn, &QPushButton::clicked, this, [this, teamNo]() { removePlayer(teamNo); });
    connect(editBtn, &QPushButton::clicked, this, [this, teamNo]() { editStats(teamNo); });
    return card;
}

void MatchDetailDialog::reload()
{
    const Match m = m_store->findMatch(m_matchId);
    m_team1Name = m.team1Name;
    m_team2Name = m.team2Name;

    m_title->setText(QStringLiteral("%1  VS  %2").arg(m.team1Name, m.team2Name));
    m_score->setText(m.scoreText());
    m_meta->setText(QStringLiteral("%1 · %2 · 胜者：%3")
                        .arg(m.dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm")),
                             m.location.isEmpty() ? QStringLiteral("未填写地点") : m.location,
                             m.winnerText()));

    if (m_team1Label)
        m_team1Label->setText(QStringLiteral("%1（%2 人）").arg(m.team1Name).arg(m.team1Players.size()));
    if (m_team2Label)
        m_team2Label->setText(QStringLiteral("%1（%2 人）").arg(m.team2Name).arg(m.team2Players.size()));

    fillStatsTable(m_table1, m.team1Players);
    fillStatsTable(m_table2, m.team2Players);
}

void MatchDetailDialog::editMatchInfo()
{
    MatchEditDialog dlg(m_store->teams(), this);
    dlg.setMatch(m_store->findMatch(m_matchId));
    if (dlg.exec() != QDialog::Accepted)
        return;

    const Match m = dlg.match();
    if (m.id != m_matchId && m_store->matchExists(m.id)) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("场次编号 %1 已存在").arg(m.id));
        return;
    }
    if (!m_store->updateMatch(m_matchId, m)) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("保存失败"));
        return;
    }
    // 编号可能被改过，之后按新编号查找
    m_matchId = m.id;
    reload();
}

void MatchDetailDialog::addPlayer(int teamNo)
{
    const Match m = m_store->findMatch(m_matchId);

    auto contains = [](const QVector<PlayerStats> &list, const QString &id) {
        for (const PlayerStats &s : list)
            if (s.playerId == id)
                return true;
        return false;
    };

    QVector<Player> candidates;
    for (const Player &p : m_store->players()) {
        if (contains(m.team1Players, p.id) || contains(m.team2Players, p.id))
            continue;
        candidates.append(p);
    }

    AddPlayerToMatchDialog dlg(candidates, m.team1Name, m.team2Name, teamNo, this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    if (!m_store->addPlayerToMatch(m_matchId, dlg.teamNo(), dlg.stats()))
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("添加失败：该球员已在本场次中"));
}

void MatchDetailDialog::removePlayer(int teamNo)
{
    QTableWidget *table = (teamNo == 1) ? m_table1 : m_table2;
    const int row = table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中要移除的球员"));
        return;
    }
    const QString playerId = table->item(row, 0)->text();
    const QString playerName = table->item(row, 1)->text();
    if (QMessageBox::question(this, QStringLiteral("确认"),
                              QStringLiteral("确定将 %1 从本场次移除？").arg(playerName))
        != QMessageBox::Yes)
        return;
    m_store->removePlayerFromMatch(m_matchId, teamNo, playerId);
}

void MatchDetailDialog::editStats(int teamNo)
{
    QTableWidget *table = (teamNo == 1) ? m_table1 : m_table2;
    const int row = table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中要编辑的球员"));
        return;
    }
    PlayerStats s;
    s.playerId = table->item(row, 0)->text();
    s.playerName = table->item(row, 1)->text();
    s.threePointers = table->item(row, 2)->text().toInt();
    s.rebounds = table->item(row, 3)->text().toInt();
    s.dunks = table->item(row, 4)->text().toInt();
    s.steals = table->item(row, 5)->text().toInt();
    if (promptStats(this, s.playerName, s))
        m_store->updatePlayerStats(m_matchId, teamNo, s.playerId, s);
}
