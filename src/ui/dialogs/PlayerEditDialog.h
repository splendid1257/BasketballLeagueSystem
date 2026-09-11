#pragma once

#include <QDialog>

#include "model/Player.h"

class QLineEdit;
class QSpinBox;
class QComboBox;
class QLabel;

// 新增 / 编辑球员对话框
class PlayerEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerEditDialog(const QStringList &teams, QWidget *parent = nullptr);

    void setPlayer(const Player &p);  // 编辑模式填充
    Player player() const;

protected:
    void accept() override;

private:
    QLineEdit *m_id = nullptr;
    QLineEdit *m_name = nullptr;
    QSpinBox *m_age = nullptr;
    QComboBox *m_team = nullptr;
    QLabel *m_titleLabel = nullptr;
    bool m_editMode = false;
};
