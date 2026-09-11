#pragma once

#include <QDateTime>
#include <QString>
#include <QVector>

// 某名球员在“某一场比赛”中的数据统计
// 对应题目九：三分球个数、篮板球个数、扣篮成功次数、抢断次数
struct PlayerStats
{
    QString playerId;    // 球员编号
    QString playerName;  // 姓名（冗余保存，便于显示）
    int threePointers = 0;  // 三分球个数
    int rebounds = 0;       // 篮板球个数
    int dunks = 0;          // 扣篮成功次数
    int steals = 0;         // 抢断次数

    // 得分：三分 3 分、扣篮 2 分（篮板/抢断不计分）
    int points() const { return threePointers * 3 + dunks * 2; }
};

// 场次信息
// 对应题目九：编号、比赛时间、比赛地点、球队一/球队二参赛队员数据集
class Match
{
public:
    QString id;         // 编号
    QDateTime dateTime;  // 比赛时间
    QString location;    // 比赛地点
    QString team1Name;   // 球队一名称
    QString team2Name;   // 球队二名称
    QVector<PlayerStats> team1Players;  // 球队一参赛队员数据集
    QVector<PlayerStats> team2Players;  // 球队二参赛队员数据集

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

    // 用于展示的比分文本，例如 “102 : 98”
    QString scoreText() const
    {
        return QStringLiteral("%1 : %2").arg(team1Points()).arg(team2Points());
    }

    // 胜者名称（平局返回“平局”）
    QString winnerText() const
    {
        if (team1Points() == team2Points())
            return QStringLiteral("平局");
        return team1Points() > team2Points() ? team1Name : team2Name;
    }
};
