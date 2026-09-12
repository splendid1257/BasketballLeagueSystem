#pragma once

#include <QDialog>
#include <QVector>

#include "model/Match.h"
#include "model/Player.h"

class QComboBox;
class QSpinBox;

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
    PlayerStats stats() const;

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
