#pragma once

#include <QWidget>

#include "storage/DataStore.h"

class QTableWidget;
class QComboBox;
class QLabel;

// 数据排行榜：得分 / 三分 / 篮板 / 扣篮 / 抢断
class StatsPage : public QWidget
{
    Q_OBJECT

public:
    explicit StatsPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();

private:
    void reload();
    DataStore::Board currentBoard() const;

    DataStore *m_store = nullptr;
    QComboBox *m_board = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel *m_subtitle = nullptr;
};
