#pragma once

#include <QDialog>

#include "model/Team.h"

class QLineEdit;
class QLabel;

// 新增 / 编辑球队对话框
class TeamEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TeamEditDialog(QWidget *parent = nullptr);

    void setTeam(const Team &t);  // 编辑模式填充
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
