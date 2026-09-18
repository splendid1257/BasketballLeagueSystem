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
    // 连接 DataStore::changed 的统一刷新入口：重建球队列表并恢复选中
    void refresh();

private:
    // 当前选中球队变化时重算花名册（refresh 恢复选中后也会触发）
    void onTeamSelected();
    void onAddTeam();
    void onEditTeam();
    void onDeleteTeam();
    void onEditPlayer();

    DataStore *m_store = nullptr;
    QListWidget *m_teamList = nullptr;
    QTableWidget *m_roster = nullptr;
    QLabel *m_teamTitle = nullptr;
    QLabel *m_teamMeta = nullptr;
};
