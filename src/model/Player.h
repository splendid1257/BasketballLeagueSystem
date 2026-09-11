#pragma once

#include <QString>

// 参赛队员（全局球员档案）
// 对应题目九“参赛队员信息”：编号、姓名、年龄 + 所属球队（增强：用于花名册/球队维度）
struct Player
{
    QString id;    // 编号（全局唯一）
    QString name;  // 姓名
    int age = 0;   // 年龄
    QString team;  // 所属球队（增强字段）

    bool isValid() const
    {
        return !id.trimmed().isEmpty() && !name.trimmed().isEmpty();
    }
};
