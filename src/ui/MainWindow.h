#pragma once

#include <QMainWindow>

class DataStore;
class QStackedWidget;
class QButtonGroup;
class QLabel;
class QPushButton;
class QCloseEvent;
class DashboardPage;
class MatchesPage;
class PlayersPage;
class TeamsPage;
class StatsPage;
class PlayerDetailPage;

// 主窗口采用“侧边栏按钮组 + 堆叠页面”导航：所有页面常驻，按钮仅切换索引
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(DataStore *store, const QString &username, QWidget *parent = nullptr);

    void setUser(const QString &username);

signals:
    void logoutRequested();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QWidget *buildSidebar();
    QWidget *buildHeader();
    QPushButton *makeNavButton(const QString &text, int index);
    void navigate(int index);
    void showMatchDetail(const QString &matchId);
    void showPlayerDetail(const QString &playerId);

    DataStore *m_store = nullptr;
    QString m_username;

    // 导航按钮组与堆叠页面，按钮 id 与页面索引一一对应（见 cpp 的 PageIndex）
    QStackedWidget *m_stack = nullptr;
    QButtonGroup *m_navGroup = nullptr;
    QLabel *m_headerTitle = nullptr;
    QLabel *m_headerUser = nullptr;
    QLabel *m_userLabel = nullptr;

    // 六个页面作为成员常驻，navigate 按索引 refresh 而非重建
    DashboardPage *m_dashboard = nullptr;
    MatchesPage *m_matches = nullptr;
    PlayersPage *m_players = nullptr;
    TeamsPage *m_teams = nullptr;
    StatsPage *m_stats = nullptr;
    PlayerDetailPage *m_playerDetail = nullptr;
};
