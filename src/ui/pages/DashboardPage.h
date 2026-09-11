#pragma once

#include <QWidget>

class DataStore;
class StatCard;
class QTableWidget;

// 首页概览：核心指标卡片 + 近期比赛 + 得分榜
class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(DataStore *store, QWidget *parent = nullptr);

public slots:
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
