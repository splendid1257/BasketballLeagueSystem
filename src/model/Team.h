#pragma once

#include <QString>

// 球队以 name 作为唯一标识（区别于 Player/Match，无独立 id）
struct Team
{
    QString name;
    QString city;
    QString coach;
    QString arena;

    bool isValid() const { return !name.trimmed().isEmpty(); }
};
