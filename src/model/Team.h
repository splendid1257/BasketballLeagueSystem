#pragma once

#include <QString>

struct Team
{
    QString name;
    QString city;
    QString coach;
    QString arena;

    bool isValid() const { return !name.trimmed().isEmpty(); }
};
