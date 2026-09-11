#pragma once

#include <QWidget>

class DataStore;
class QTableWidget;
class QLineEdit;
class EmptyStateLabel;

// 场次管理：列表 + 搜索 + 增删改查
class MatchesPage : public QWidget
{
    Q_OBJECT

public:
    explicit MatchesPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();

signals:
    void matchDetailRequested(const QString &matchId);

private:
    void onAdd();
    void onEdit();
    void onDelete();
    void onDetail();
    QString selectedMatchId() const;
    void applyFilter();

    DataStore *m_store = nullptr;
    QTableWidget *m_table = nullptr;
    QLineEdit *m_search = nullptr;
    EmptyStateLabel *m_empty = nullptr;
};
