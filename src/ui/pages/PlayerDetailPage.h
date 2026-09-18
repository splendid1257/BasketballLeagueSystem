#pragma once

#include <QWidget>

class DataStore;
class StatCard;
class QTableWidget;
class QLabel;

// 球员详情页：展示单个球员的生涯合计指标与逐场出场记录
class PlayerDetailPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerDetailPage(DataStore *store, QWidget *parent = nullptr);

    // 由导航层进入详情时调用，传入目标球员的全局唯一 id
    void setPlayer(const QString &playerId);
    QString playerName() const { return m_playerName; }

public slots:
    void refresh();

signals:
    // 点击返回时发出，由父级导航层接管返回列表页
    void backRequested();

private:
    DataStore *m_store = nullptr;
    QString m_playerId;
    QString m_playerName;

    QLabel *m_name = nullptr;
    QLabel *m_sub = nullptr;
    StatCard *m_cardPoints = nullptr;
    StatCard *m_cardTp = nullptr;
    StatCard *m_cardRb = nullptr;
    StatCard *m_cardDk = nullptr;
    StatCard *m_cardSt = nullptr;
    QTableWidget *m_log = nullptr;
};
