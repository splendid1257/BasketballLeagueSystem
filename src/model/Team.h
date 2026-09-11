#pragma once

#include <QString>

// 球队档案
struct Team
{
    QString name;   // 队名（唯一）
    QString city;   // 所在城市
    QString coach;  // 主教练
    QString arena;  // 主场

    bool isValid() const { return !name.trimmed().isEmpty(); }
};
