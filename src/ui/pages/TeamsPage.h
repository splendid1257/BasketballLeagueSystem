#pragma once

#include <QWidget>

class DataStore;
class QListWidget;
class QTableWidget;
class QLabel;

class TeamsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeamsPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();

private:
    void onTeamSelected();
    void onAddTeam();
    void onEditTeam();
    void onDeleteTeam();

    DataStore *m_store = nullptr;
    QListWidget *m_teamList = nullptr;
    QTableWidget *m_roster = nullptr;
    QLabel *m_teamTitle = nullptr;
    QLabel *m_teamMeta = nullptr;
};
