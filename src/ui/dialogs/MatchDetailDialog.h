#pragma once

#include <QDialog>

class DataStore;
class QTableWidget;
class QLabel;
class QWidget;

class MatchDetailDialog : public QDialog
{
    Q_OBJECT

public:
    MatchDetailDialog(DataStore *store, const QString &matchId, QWidget *parent = nullptr);

private:
    QWidget *buildTeamPanel(int teamNo, QTableWidget *&table);
    void reload();
    void editMatchInfo();
    void addPlayer(int teamNo);
    void removePlayer(int teamNo);
    void editStats(int teamNo);

    DataStore *m_store = nullptr;
    QString m_matchId;
    QLabel *m_title = nullptr;
    QLabel *m_score = nullptr;
    QLabel *m_meta = nullptr;
    QTableWidget *m_table1 = nullptr;
    QTableWidget *m_table2 = nullptr;
    QLabel *m_team1Label = nullptr;
    QLabel *m_team2Label = nullptr;
    QString m_team1Name;
    QString m_team2Name;
};
