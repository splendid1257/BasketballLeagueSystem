#pragma once

#include <QWidget>

class DataStore;
class QTableWidget;
class QLineEdit;

// 球员管理：列表 + 搜索 + 增删改查
class PlayersPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayersPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();

signals:
    void playerDetailRequested(const QString &playerId);

private:
    void onAdd();
    void onEdit();
    void onDelete();
    void onDetail();
    QString selectedPlayerId() const;
    void applyFilter();

    DataStore *m_store = nullptr;
    QTableWidget *m_table = nullptr;
    QLineEdit *m_search = nullptr;
};
