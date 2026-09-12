#pragma once

#include <QDialog>

#include "model/Team.h"

class QLineEdit;
class QLabel;

class TeamEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TeamEditDialog(QWidget *parent = nullptr);

    void setTeam(const Team &t);
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
