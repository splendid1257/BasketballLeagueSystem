#include "ui/MainWindow.h"

#include "storage/DataStore.h"
#include "ui/dialogs/MatchDetailDialog.h"
#include "ui/pages/DashboardPage.h"
#include "ui/pages/MatchesPage.h"
#include "ui/pages/PlayerDetailPage.h"
#include "ui/pages/PlayersPage.h"
#include "ui/pages/StatsPage.h"
#include "ui/pages/TeamsPage.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCloseEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace {
enum PageIndex { Dashboard = 0, Matches = 1, Players = 2, Teams = 3, Stats = 4, PlayerDetail = 5 };
}

MainWindow::MainWindow(DataStore *store, const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , m_store(store)
    , m_username(username)
{
    setWindowTitle(QStringLiteral("篮球联赛管理系统"));
    resize(1320, 840);
    setMinimumSize(1120, 720);

    auto *central = new QWidget(this);
    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    root->addWidget(buildSidebar());

    auto *right = new QWidget(central);
    auto *rv = new QVBoxLayout(right);
    rv->setContentsMargins(0, 0, 0, 0);
    rv->setSpacing(0);
    rv->addWidget(buildHeader());

    m_stack = new QStackedWidget(right);
    m_dashboard = new DashboardPage(m_store, m_stack);
    m_matches = new MatchesPage(m_store, m_stack);
    m_players = new PlayersPage(m_store, m_stack);
    m_teams = new TeamsPage(m_store, m_stack);
    m_stats = new StatsPage(m_store, m_stack);
    m_playerDetail = new PlayerDetailPage(m_store, m_stack);

    m_stack->addWidget(m_dashboard);
    m_stack->addWidget(m_matches);
    m_stack->addWidget(m_players);
    m_stack->addWidget(m_teams);
    m_stack->addWidget(m_stats);
    m_stack->addWidget(m_playerDetail);
    rv->addWidget(m_stack, 1);

    root->addWidget(right, 1);
    setCentralWidget(central);

    connect(m_matches, &MatchesPage::matchDetailRequested, this, &MainWindow::showMatchDetail);
    connect(m_players, &PlayersPage::playerDetailRequested, this, &MainWindow::showPlayerDetail);
    connect(m_playerDetail, &PlayerDetailPage::backRequested, this, [this]() { navigate(Players); });

    navigate(Dashboard);
}

QWidget *MainWindow::buildSidebar()
{
    auto *sidebar = new QFrame(this);
    sidebar->setObjectName(QStringLiteral("Sidebar"));
    sidebar->setFixedWidth(232);

    auto *v = new QVBoxLayout(sidebar);
    v->setContentsMargins(0, 22, 0, 18);
    v->setSpacing(4);

    auto *brand = new QLabel(QStringLiteral("🏀  篮球联赛管理系统"), sidebar);
    brand->setStyleSheet(QStringLiteral("font-size:16px; font-weight:800; color:#FFFFFF; padding:0 18px;"));
    auto *brandSub = new QLabel(QStringLiteral("League Management"), sidebar);
    brandSub->setStyleSheet(QStringLiteral("color:#6B778C; font-size:11px; letter-spacing:2px; padding:0 18px 12px 18px;"));
    v->addWidget(brand);
    v->addWidget(brandSub);

    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);

    const QStringList names = {QStringLiteral("  首页概览"), QStringLiteral("  场次管理"),
                               QStringLiteral("  球员管理"), QStringLiteral("  球队花名册"),
                               QStringLiteral("  数据排行榜")};
    for (int i = 0; i < names.size(); ++i) {
        auto *btn = makeNavButton(names.at(i), i);
        m_navGroup->addButton(btn, i);
        v->addWidget(btn);
    }

    v->addStretch();

    auto *divider = new QFrame(sidebar);
    divider->setFixedHeight(1);
    divider->setStyleSheet(QStringLiteral("background:#1E2634; margin:0 16px;"));
    v->addWidget(divider);
    v->addSpacing(8);

    m_userLabel = new QLabel(sidebar);
    m_userLabel->setStyleSheet(QStringLiteral("color:#9AA7BC; padding:0 18px;"));
    v->addWidget(m_userLabel);

    auto *logout = new QPushButton(QStringLiteral("退出登录"), sidebar);
    logout->setObjectName(QStringLiteral("Ghost"));
    logout->setCursor(Qt::PointingHandCursor);
    logout->setStyleSheet(QStringLiteral("margin:8px 16px 0 16px;"));
    connect(logout, &QPushButton::clicked, this, &MainWindow::logoutRequested);
    v->addWidget(logout);

    setUser(m_username);
    return sidebar;
}

QPushButton *MainWindow::makeNavButton(const QString &text, int index)
{
    auto *btn = new QPushButton(text, this);
    btn->setObjectName(QStringLiteral("NavItem"));
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, [this, index]() { navigate(index); });
    return btn;
}

QWidget *MainWindow::buildHeader()
{
    auto *header = new QFrame(this);
    header->setFixedHeight(64);
    header->setStyleSheet(QStringLiteral("background:#0E121A; border-bottom:1px solid #1E2634;"));

    auto *h = new QHBoxLayout(header);
    h->setContentsMargins(28, 0, 28, 0);

    m_headerTitle = new QLabel(QStringLiteral("首页概览"), header);
    m_headerTitle->setObjectName(QStringLiteral("H2"));
    h->addWidget(m_headerTitle);
    h->addStretch();

    auto *user = new QLabel(header);
    user->setText(QStringLiteral("👤  %1").arg(m_username));
    user->setStyleSheet(QStringLiteral("color:#9AA7BC; font-weight:600;"));
    h->addWidget(user);
    return header;
}

void MainWindow::setUser(const QString &username)
{
    m_username = username;
    if (m_userLabel)
        m_userLabel->setText(QStringLiteral("当前用户：%1").arg(username));
}

void MainWindow::navigate(int index)
{
    m_stack->setCurrentIndex(index);

    static const QStringList titles = {QStringLiteral("首页概览"), QStringLiteral("场次管理"),
                                       QStringLiteral("球员管理"), QStringLiteral("球队与花名册"),
                                       QStringLiteral("数据排行榜"), QStringLiteral("球员详情")};
    if (index >= 0 && index < titles.size())
        m_headerTitle->setText(titles.at(index));

    if (index <= Stats && m_navGroup->button(index))
        m_navGroup->button(index)->setChecked(true);
    else if (index == PlayerDetail && m_navGroup->button(Players))
        m_navGroup->button(Players)->setChecked(true);

    switch (index) {
    case Dashboard: m_dashboard->refresh(); break;
    case Matches: m_matches->refresh(); break;
    case Players: m_players->refresh(); break;
    case Teams: m_teams->refresh(); break;
    case Stats: m_stats->refresh(); break;
    case PlayerDetail: m_playerDetail->refresh(); break;
    default: break;
    }
}

void MainWindow::showMatchDetail(const QString &matchId)
{
    MatchDetailDialog dlg(m_store, matchId, this);
    dlg.exec();
    m_matches->refresh();
}

void MainWindow::showPlayerDetail(const QString &playerId)
{
    m_playerDetail->setPlayer(playerId);
    m_headerTitle->setText(QStringLiteral("球员详情 · %1").arg(m_playerDetail->playerName()));
    navigate(PlayerDetail);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    qApp->quit();
}
