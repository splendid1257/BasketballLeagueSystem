#pragma once

#include <QDialog>
#include <QVector>

#include "model/Match.h"
#include "model/Player.h"

class QComboBox;
class QSpinBox;

// 向场次添加参赛队员的编辑对话框：候选已由调用方排除在册者，
// 本类只负责录入与校验并暴露结果，持久化交由 DataStore 完成
class AddPlayerToMatchDialog : public QDialog
{
    Q_OBJECT

public:
    AddPlayerToMatchDialog(const QVector<Player> &candidates,
                           const QString &team1Name,
                           const QString &team2Name,
                           int presetTeam,
                           QWidget *parent = nullptr);

    int teamNo() const;  // 1 或 2
    PlayerStats stats() const;  // 由当前选择构建，未落盘

protected:
    void accept() override;

private:
    QComboBox *m_team = nullptr;
    QComboBox *m_player = nullptr;
    QSpinBox *m_tp = nullptr;
    QSpinBox *m_rb = nullptr;
    QSpinBox *m_dk = nullptr;
    QSpinBox *m_st = nullptr;
};
