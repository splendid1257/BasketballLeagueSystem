#pragma once

#include <QDateTime>
#include <QString>
#include <QVector>

struct PlayerStats
{
    QString playerId;
    QString playerName;  // 冗余保存，便于显示
    int threePointers = 0;
    int rebounds = 0;
    int dunks = 0;
    int steals = 0;

    // 得分：三分 3 分、扣篮 2 分（篮板/抢断不计分）
    int points() const { return threePointers * 3 + dunks * 2; }
};

class Match
{
public:
    QString id;
    QDateTime dateTime;
    QString location;
    QString team1Name;
    QString team2Name;
    QVector<PlayerStats> team1Players;
    QVector<PlayerStats> team2Players;

    bool isValid() const { return !id.trimmed().isEmpty(); }

    static int sumPoints(const QVector<PlayerStats> &list)
    {
        int total = 0;
        for (const PlayerStats &s : list)
            total += s.points();
        return total;
    }

    int team1Points() const { return sumPoints(team1Players); }
    int team2Points() const { return sumPoints(team2Players); }

    // 形如 “102 : 98”
    QString scoreText() const
    {
        return QStringLiteral("%1 : %2").arg(team1Points()).arg(team2Points());
    }

    // 平局返回 “平局”
    QString winnerText() const
    {
        if (team1Points() == team2Points())
            return QStringLiteral("平局");
        return team1Points() > team2Points() ? team1Name : team2Name;
    }
};
