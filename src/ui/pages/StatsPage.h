#pragma once

#include <QWidget>

#include "storage/DataStore.h"

class QTableWidget;
class QComboBox;
class QLabel;

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
