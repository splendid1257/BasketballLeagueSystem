#include "ui/pages/DashboardPage.h"

#include "storage/DataStore.h"
#include "ui/widgets/StatCard.h"
#include "ui/widgets/UiUtils.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

#include <algorithm>

DashboardPage::DashboardPage(DataStore *store, QWidget *parent)
    : QWidget(parent)
    , m_store(store)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(18);

    auto *title = new QLabel(QStringLiteral("首页概览"), this);
    title->setObjectName(QStringLiteral("H1"));
    auto *sub = new QLabel(QStringLiteral("联赛整体数据一览"), this);
    sub->setObjectName(QStringLiteral("Muted"));
    root->addWidget(title);
    root->addWidget(sub);

    auto *cards = new QHBoxLayout();
    cards->setSpacing(16);
    m_cardMatches = new StatCard(QStringLiteral("总场次"), QStringLiteral("#C8102E"), this);
    m_cardPlayers = new StatCard(QStringLiteral("球员总数"), QStringLiteral("#1D428A"), this);
    m_cardTeams = new StatCard(QStringLiteral("球队数"), QStringLiteral("#FDB927"), this);
    m_cardPoints = new StatCard(QStringLiteral("总得分"), QStringLiteral("#00B2A9"), this);
    for (StatCard *c : {m_cardMatches, m_cardPlayers, m_cardTeams, m_cardPoints})
        cards->addWidget(c, 1);
    root->addLayout(cards);

    auto *columns = new QHBoxLayout();
    columns->setSpacing(16);

    auto *left = new QVBoxLayout();
    left->setSpacing(8);
    auto *lTitle = new QLabel(QStringLiteral("近期比赛"), this);
    lTitle->setObjectName(QStringLiteral("H3"));
    m_recent = new QTableWidget(this);
    ui::setupTable(m_recent, {QStringLiteral("时间"), QStringLiteral("对阵"),
                              QStringLiteral("比分"), QStringLiteral("胜者")});
    left->addWidget(lTitle);
    left->addWidget(m_recent, 1);

    auto *right = new QVBoxLayout();
    right->setSpacing(8);
    auto *rTitle = new QLabel(QStringLiteral("得分榜 TOP 5"), this);
    rTitle->setObjectName(QStringLiteral("H3"));
    m_topScorers = new QTableWidget(this);
    ui::setupTable(m_topScorers, {QStringLiteral("排名"), QStringLiteral("球员"),
                                  QStringLiteral("球队"), QStringLiteral("得分")});
    right->addWidget(rTitle);
    right->addWidget(m_topScorers, 1);

    columns->addLayout(left, 3);
    columns->addLayout(right, 2);
    root->addLayout(columns, 1);

    connect(m_store, &DataStore::changed, this, &DashboardPage::refresh);
    refresh();
}

void DashboardPage::refresh()
{
    m_cardMatches->setValue(QString::number(m_store->matches().size()));
    m_cardMatches->setSubtitle(QStringLiteral("已完成/已录入"));

    m_cardPlayers->setValue(QString::number(m_store->players().size()));
    m_cardPlayers->setSubtitle(QStringLiteral("注册球员"));

    const QStringList teams = m_store->teams();
    m_cardTeams->setValue(QString::number(teams.size()));
    m_cardTeams->setSubtitle(teams.isEmpty() ? QStringLiteral("暂无球队")
                                             : QStringLiteral("覆盖 %1 支球队").arg(teams.size()));

    m_cardPoints->setValue(QString::number(m_store->totalPoints()));
    m_cardPoints->setSubtitle(QStringLiteral("全部场次合计"));

    QVector<Match> matches = m_store->matches();
    std::sort(matches.begin(), matches.end(), [](const Match &a, const Match &b) {
        return a.dateTime > b.dateTime;
    });
    const int recentCount = qMin(6, matches.size());
    m_recent->setRowCount(recentCount);
    for (int r = 0; r < recentCount; ++r) {
        const Match &m = matches.at(r);
        m_recent->setItem(r, 0, ui::item(m.dateTime.toString(QStringLiteral("MM-dd HH:mm")),
                                         Qt::AlignLeft, ui::dim()));
        m_recent->setItem(r, 1, ui::item(QStringLiteral("%1 vs %2").arg(m.team1Name, m.team2Name),
                                         Qt::AlignLeft));
        m_recent->setItem(r, 2, ui::item(m.scoreText(), Qt::AlignCenter, ui::gold()));
        m_recent->setItem(r, 3, ui::item(m.winnerText(), Qt::AlignLeft, ui::green()));
    }
    m_recent->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_recent->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_recent->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_recent->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    const auto board = m_store->leaderboard(DataStore::Board::Points);
    const int topN = qMin(5, board.size());
    m_topScorers->setRowCount(topN);
    for (int r = 0; r < topN; ++r) {
        const Player p = m_store->findPlayer(board.at(r).first);
        const QColor medal = (r == 0) ? ui::gold() : (r < 3 ? ui::green() : ui::dim());
        m_topScorers->setItem(r, 0, ui::item(QString::number(r + 1), Qt::AlignCenter, medal));
        m_topScorers->setItem(r, 1, ui::item(p.name, Qt::AlignLeft));
        m_topScorers->setItem(r, 2, ui::item(p.team, Qt::AlignLeft, ui::dim()));
        m_topScorers->setItem(r, 3, ui::item(QString::number(board.at(r).second), Qt::AlignCenter, ui::gold()));
    }
    m_topScorers->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_topScorers->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_topScorers->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_topScorers->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
}
