#pragma once

#include <QObject>
#include <QPair>
#include <QStringList>
#include <QVector>

#include "model/Match.h"
#include "model/Player.h"
#include "model/Team.h"
#include "model/User.h"

// 数据文件位于 可执行文件同级的 data/ 目录
class DataStore : public QObject
{
    Q_OBJECT

public:
    explicit DataStore(QObject *parent = nullptr);

    // 文件不存在时自动创建并写入演示数据
    bool load();
    bool save() const;

    QString dataDir() const { return m_dataDir; }

    const QVector<Player> &players() const { return m_players; }
    bool playerExists(const QString &id) const;
    bool addPlayer(const Player &p);
    bool updatePlayer(const QString &oldId, const Player &p);
    bool removePlayer(const QString &id);
    Player findPlayer(const QString &id) const;  // 不存在返回默认对象
    QStringList teams() const;  // 全部球队名（球队档案 + 球员所属，去重排序）

    const QVector<Team> &teamRecords() const { return m_teams; }
    bool teamExists(const QString &name) const;
    bool addTeam(const Team &t);
    bool updateTeam(const QString &oldName, const Team &t);
    bool removeTeam(const QString &name);
    Team findTeam(const QString &name) const;  // 不存在返回默认对象

    const QVector<Match> &matches() const { return m_matches; }
    bool matchExists(const QString &id) const;
    bool addMatch(const Match &m);
    bool updateMatch(const QString &oldId, const Match &m);
    bool removeMatch(const QString &id);
    Match findMatch(const QString &id) const;  // 不存在返回默认对象

    // 向某场次的某支球队增/删参赛队员；teamNo 为 1 或 2
    bool addPlayerToMatch(const QString &matchId, int teamNo, const PlayerStats &s);
    bool removePlayerFromMatch(const QString &matchId, int teamNo, const QString &playerId);
    bool updatePlayerStats(const QString &matchId, int teamNo, const QString &playerId,
                           const PlayerStats &s);

    // 跨所有场次合计
    PlayerStats careerTotals(const QString &playerId) const;
    // 球员出场场次数
    int playerGameCount(const QString &playerId) const;
    // 球员出场记录：<场次编号, 该场数据>
    QVector<QPair<QString, PlayerStats>> playerMatchLog(const QString &playerId) const;

    // 排行榜：<球员编号, 数值>，按数值降序
    enum class Board { Points, ThreePointers, Rebounds, Dunks, Steals };
    QVector<QPair<QString, int>> leaderboard(Board board) const;

    int totalPoints() const;

    const QVector<User> &users() const { return m_users; }
    bool userExists(const QString &username) const;
    void addUser(const User &u);
    User findUser(const QString &username) const;

signals:
    void changed();

private:
    bool loadPlayers();
    bool loadMatches();
    bool loadTeams();
    bool loadUsers();
    void seedDemoData();
    void ensureDataDir();

    QVector<Player> m_players;
    QVector<Match> m_matches;
    QVector<Team> m_teams;
    QVector<User> m_users;

    QString m_dataDir;
    QString m_playersFile;
    QString m_matchesFile;
    QString m_teamsFile;
    QString m_usersFile;
};
