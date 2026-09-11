#pragma once

#include <QDialog>

#include "model/Match.h"

class QLineEdit;
class QDateTimeEdit;
class QComboBox;
class QLabel;

// 新增 / 编辑场次基本信息对话框（参赛队员在“场次详情”中维护）
class MatchEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MatchEditDialog(const QStringList &teams, QWidget *parent = nullptr);

    void setMatch(const Match &m);  // 编辑模式填充（含已有球员数据）
    Match match() const;

protected:
    void accept() override;

private:
    QLineEdit *m_id = nullptr;
    QDateTimeEdit *m_dt = nullptr;
    QLineEdit *m_loc = nullptr;
    QComboBox *m_team1 = nullptr;
    QComboBox *m_team2 = nullptr;
    QLabel *m_titleLabel = nullptr;

    QVector<PlayerStats> m_team1Players;
    QVector<PlayerStats> m_team2Players;
    bool m_editMode = false;
};
