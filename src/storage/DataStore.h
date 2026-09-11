#pragma once

#include <QObject>
#include <QPair>
#include <QStringList>
#include <QVector>

#include "model/Match.h"
#include "model/Player.h"
#include "model/User.h"

// 数据仓储：负责所有实体的增删改查 + 文件读写（JSON）
// 数据文件位于 可执行文件同级的 data/ 目录
class DataStore : public QObject
{
    Q_OBJECT

public:
    explicit DataStore(QObject *parent = nullptr);

    // 载入全部数据；文件不存在时自动创建并写入演示数据
    bool load();
    bool save() const;

    QString dataDir() const { return m_dataDir; }

    // ---------------- 球员 ----------------
    const QVector<Player> &players() const { return m_players; }
    bool playerExists(const QString &id) const;
    bool addPlayer(const Player &p);
    bool updatePlayer(const QString &oldId, const Player &p);
    bool removePlayer(const QString &id);
    Player findPlayer(const QString &id) const;  // 不存在返回默认对象
    QStringList teams() const;                   // 去重后的球队列表

    // ---------------- 场次 ----------------
    const QVector<Match> &matches() const { return m_matches; }
    bool matchExists(const QString &id) const;
    bool addMatch(const Match &m);
    bool updateMatch(const QString &oldId, const Match &m);
    bool removeMatch(const QString &id);
    Match findMatch(const QString &id) const;  // 不存在返回默认对象

    // 向某场次的某支球队增/删参赛队员；teamNo 为 1 或 2
    bool addPlayerToMatch(const QString &matchId, int teamNo, const PlayerStats &s);
    bool removePlayerFromMatch(const QString &matchId, int teamNo, const QString &playerId);

    // ---------------- 统计聚合 ----------------
    // 球员生涯合计（跨所有场次）
    PlayerStats careerTotals(const QString &playerId) const;
    // 球员出场记录：<场次编号, 该场数据>
    QVector<QPair<QString, PlayerStats>> playerMatchLog(const QString &playerId) const;

    // 排行榜：<球员编号, 数值>，按数值降序
    enum class Board { Points, ThreePointers, Rebounds, Dunks, Steals };
    QVector<QPair<QString, int>> leaderboard(Board board) const;

    // 全联盟总得分
    int totalPoints() const;

    // ---------------- 用户 ----------------
    const QVector<User> &users() const { return m_users; }
    bool userExists(const QString &username) const;
    void addUser(const User &u);
    User findUser(const QString &username) const;

signals:
    void changed();

private:
    bool loadPlayers();
    bool loadMatches();
    bool loadUsers();
    void seedDemoData();
    void ensureDataDir();

    QVector<Player> m_players;
    QVector<Match> m_matches;
    QVector<User> m_users;

    QString m_dataDir;
    QString m_playersFile;
    QString m_matchesFile;
    QString m_usersFile;
};
