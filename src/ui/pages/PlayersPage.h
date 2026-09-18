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

class PlayersPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayersPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    // 由 DataStore::changed 驱动的统一刷新入口：重建筛选并重绘当前页
    void refresh();

signals:
    // 请求跳转到球员详情（参数为球员编号），导航职责留给主窗口
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

    // 过滤后的候选集：renderPage 只在该集合上分页，不改动数据源
    QVector<Player> m_filtered;
    // 分页状态：m_page 从 0 起，m_pageSize 为每页条数
    int m_page = 0;
    int m_pageSize = 10;
};
