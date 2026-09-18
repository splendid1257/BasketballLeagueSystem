#pragma once

#include <QString>

// 球员档案；id 全局唯一，是关联单场统计 PlayerStats 的键
struct Player
{
    QString id;
    QString name;
    int age = 0;
    QString team;
    int number = 0;    // 球衣号码
    QString position;  // 场上位置（PG/SG/SF/PF/C）
    int heightCm = 0;
    int weightKg = 0;
    QString country;

    bool isValid() const
    {
        return !id.trimmed().isEmpty() && !name.trimmed().isEmpty();
    }
};
