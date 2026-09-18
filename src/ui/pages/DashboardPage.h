#pragma once

#include <QWidget>

class DataStore;
class StatCard;
class QTableWidget;

// 首页概览页：汇总联赛整体指标，展示近期比赛与得分榜前五
class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    // 数据变更后由 DataStore::changed 信号驱动重算
    void refresh();

private:
    DataStore *m_store = nullptr;
    StatCard *m_cardMatches = nullptr;
    StatCard *m_cardPlayers = nullptr;
    StatCard *m_cardTeams = nullptr;
    StatCard *m_cardPoints = nullptr;
    QTableWidget *m_recent = nullptr;
    QTableWidget *m_topScorers = nullptr;
};
