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

    QStackedWidget *m_stack = nullptr;
    QButtonGroup *m_navGroup = nullptr;
    QLabel *m_headerTitle = nullptr;
    QLabel *m_headerUser = nullptr;
    QLabel *m_userLabel = nullptr;

    DashboardPage *m_dashboard = nullptr;
    MatchesPage *m_matches = nullptr;
    PlayersPage *m_players = nullptr;
    TeamsPage *m_teams = nullptr;
    StatsPage *m_stats = nullptr;
    PlayerDetailPage *m_playerDetail = nullptr;
};
