#pragma once

#include <QWidget>

#include "storage/DataStore.h"

class QTableWidget;
class QComboBox;
class QLabel;

// 数据排行榜页：对球员生涯合计按选定榜单降序排名并展示明细
class StatsPage : public QWidget
{
    Q_OBJECT

public:
    explicit StatsPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();

private:
    // 切换榜单或数据变更后重建整个表格（行数随榜单规模变化）
    void reload();
    DataStore::Board currentBoard() const;

    DataStore *m_store = nullptr;
    QComboBox *m_board = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel *m_subtitle = nullptr;
};
