#include "ui/pages/PlayerDetailPage.h"

#include "storage/DataStore.h"
#include "ui/widgets/StatCard.h"
#include "ui/widgets/UiUtils.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QVBoxLayout>

PlayerDetailPage::PlayerDetailPage(DataStore *store, QWidget *parent)
    : QWidget(parent)
    , m_store(store)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    auto *top = new QHBoxLayout();
    auto *back = new QPushButton(QStringLiteral("← 返回球员列表"), this);
    back->setObjectName(QStringLiteral("Ghost"));
    back->setCursor(Qt::PointingHandCursor);
    top->addWidget(back);
    top->addStretch();
    root->addLayout(top);

    auto *head = new QVBoxLayout();
    head->setSpacing(2);
    m_name = new QLabel(QStringLiteral("球员"), this);
    m_name->setObjectName(QStringLiteral("H1"));
    m_sub = new QLabel(this);
    m_sub->setObjectName(QStringLiteral("Muted"));
    head->addWidget(m_name);
    head->addWidget(m_sub);
    root->addLayout(head);

    auto *cards = new QHBoxLayout();
    cards->setSpacing(14);
    m_cardPoints = new StatCard(QStringLiteral("生涯总得分"), QStringLiteral("#C8102E"), this);
    m_cardTp = new StatCard(QStringLiteral("三分球"), QStringLiteral("#1D428A"), this);
    m_cardRb = new StatCard(QStringLiteral("篮板球"), QStringLiteral("#FDB927"), this);
    m_cardDk = new StatCard(QStringLiteral("扣篮成功"), QStringLiteral("#00B2A9"), this);
    m_cardSt = new StatCard(QStringLiteral("抢断"), QStringLiteral("#7C4DFF"), this);
    for (StatCard *c : {m_cardPoints, m_cardTp, m_cardRb, m_cardDk, m_cardSt})
        cards->addWidget(c, 1);
    root->addLayout(cards);

    auto *logTitle = new QLabel(QStringLiteral("出场记录"), this);
    logTitle->setObjectName(QStringLiteral("H3"));
    root->addWidget(logTitle);

    m_log = new QTableWidget(this);
    ui::setupTable(m_log, {QStringLiteral("场次"), QStringLiteral("时间"),
                           QStringLiteral("地点"), QStringLiteral("对阵"),
                           QStringLiteral("三分"), QStringLiteral("篮板"),
                           QStringLiteral("扣篮"), QStringLiteral("抢断"),
                           QStringLiteral("得分")});
    ui::autoSizeColumns(m_log, {3});
    for (int c = 0; c < m_log->columnCount(); ++c)
        ui::alignHeader(m_log, c, (c == 2 || c == 3) ? Qt::AlignLeft : Qt::AlignCenter);
    root->addWidget(m_log, 1);

    connect(back, &QPushButton::clicked, this, &PlayerDetailPage::backRequested);
    connect(m_store, &DataStore::changed, this, &PlayerDetailPage::refresh);
}

void PlayerDetailPage::setPlayer(const QString &playerId)
{
    m_playerId = playerId;
    refresh();
}

void PlayerDetailPage::refresh()
{
    if (m_playerId.isEmpty())
        return;

    const Player p = m_store->findPlayer(m_playerId);
    m_playerName = p.name;
    const PlayerStats total = m_store->careerTotals(m_playerId);
    const int games = m_store->playerMatchLog(m_playerId).size();

    m_name->setText(p.name.isEmpty() ? QStringLiteral("（已删除的球员）") : p.name);
    QStringList bits;
    bits << QStringLiteral("编号 %1").arg(p.id.isEmpty() ? QStringLiteral("-") : p.id);
    bits << (p.team.isEmpty() ? QStringLiteral("无球队") : p.team);
    if (!p.position.isEmpty())
        bits << p.position;
    bits << QStringLiteral("%1 岁").arg(p.age);
    if (p.heightCm > 0)
        bits << QStringLiteral("%1 cm").arg(p.heightCm);
    if (p.weightKg > 0)
        bits << QStringLiteral("%1 kg").arg(p.weightKg);
    if (!p.country.isEmpty())
        bits << p.country;
    bits << QStringLiteral("出场 %1 场").arg(games);
    m_sub->setText(bits.join(QStringLiteral(" · ")));

    m_cardPoints->setValue(QString::number(total.points()));
    m_cardTp->setValue(QString::number(total.threePointers));
    m_cardRb->setValue(QString::number(total.rebounds));
    m_cardDk->setValue(QString::number(total.dunks));
    m_cardSt->setValue(QString::number(total.steals));

    const auto log = m_store->playerMatchLog(m_playerId);
    m_log->setRowCount(log.size());
    for (int r = 0; r < log.size(); ++r) {
        const QString matchId = log.at(r).first;
        const PlayerStats &s = log.at(r).second;
        const Match m = m_store->findMatch(matchId);

        m_log->setItem(r, 0, ui::item(matchId, Qt::AlignCenter, ui::gold()));
        m_log->setItem(r, 1, ui::item(m.dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm"))));
        m_log->setItem(r, 2, ui::item(m.location, Qt::AlignLeft, ui::dim()));
        m_log->setItem(r, 3, ui::item(QStringLiteral("%1 vs %2").arg(m.team1Name, m.team2Name), Qt::AlignLeft));
        m_log->setItem(r, 4, ui::item(QString::number(s.threePointers)));
        m_log->setItem(r, 5, ui::item(QString::number(s.rebounds)));
        m_log->setItem(r, 6, ui::item(QString::number(s.dunks)));
        m_log->setItem(r, 7, ui::item(QString::number(s.steals)));
        m_log->setItem(r, 8, ui::item(QString::number(s.points()), Qt::AlignCenter, ui::green()));
    }
}
