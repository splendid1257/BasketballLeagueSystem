#pragma once

#include <QDialog>

#include "model/Team.h"

class QLineEdit;
class QLabel;

// 球队新增/编辑对话框：录入并校验，经 team() 暴露结果，不落库
class TeamEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TeamEditDialog(QWidget *parent = nullptr);

    void setTeam(const Team &t);  // 编辑时预填
    Team team() const;

protected:
    void accept() override;

private:
    QLineEdit *m_name = nullptr;
    QLineEdit *m_city = nullptr;
    QLineEdit *m_coach = nullptr;
    QLineEdit *m_arena = nullptr;
    QLabel *m_titleLabel = nullptr;
};
