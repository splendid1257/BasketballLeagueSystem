#pragma once

#include <QVector>
#include <QWidget>

#include "model/Player.h"

class DataStore;
class QTableWidget;
class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;
class EmptyStateLabel;

// 球员管理：搜索 + 球队/位置筛选 + 分页 + 增删改查（仿 NBA 球员列表页）
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
    void rebuildFilters();
    void applyFilter();
    void renderPage();

    DataStore *m_store = nullptr;
    QTableWidget *m_table = nullptr;
    QLineEdit *m_search = nullptr;
    QComboBox *m_teamFilter = nullptr;
    QComboBox *m_positionFilter = nullptr;
    EmptyStateLabel *m_empty = nullptr;
    QLabel *m_countLabel = nullptr;
    QPushButton *m_prevBtn = nullptr;
    QPushButton *m_nextBtn = nullptr;

    QVector<Player> m_filtered;
    int m_page = 0;
    int m_pageSize = 10;
};
