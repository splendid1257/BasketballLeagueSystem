#pragma once

#include <QWidget>

class DataStore;
class QListWidget;
class QTableWidget;
class QLabel;

// 球队与花名册：左侧球队列表，右侧展示该队全部球员
class TeamsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeamsPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();

private:
    void onTeamSelected();

    DataStore *m_store = nullptr;
    QListWidget *m_teamList = nullptr;
    QTableWidget *m_roster = nullptr;
    QLabel *m_teamTitle = nullptr;
    QLabel *m_teamMeta = nullptr;
};
