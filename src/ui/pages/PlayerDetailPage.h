#pragma once

#include <QWidget>

class DataStore;
class StatCard;
class QTableWidget;
class QLabel;

// 球员详情页：生涯合计数据 + 各场次出场记录
class PlayerDetailPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerDetailPage(DataStore *store, QWidget *parent = nullptr);

    void setPlayer(const QString &playerId);
    QString playerName() const { return m_playerName; }

public slots:
    void refresh();

signals:
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
