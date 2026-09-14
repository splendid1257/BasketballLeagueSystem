#pragma once

#include <QColor>
#include <QHeaderView>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <initializer_list>

namespace ui {

inline void setupTable(QTableWidget *t, const QStringList &headers)
{
    t->setColumnCount(headers.size());
    t->setHorizontalHeaderLabels(headers);
    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setAlternatingRowColors(true);
    t->setShowGrid(false);
    t->horizontalHeader()->setHighlightSections(false);
    t->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

inline void autoSizeColumns(QTableWidget *t, std::initializer_list<int> stretchCols = {})
{
    for (int c = 0; c < t->columnCount(); ++c) {
        bool stretch = false;
        for (int s : stretchCols) {
            if (s == c) { stretch = true; break; }
        }
        t->horizontalHeader()->setSectionResizeMode(
            c, stretch ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
    }
}

inline void alignHeader(QTableWidget *t, int column, Qt::Alignment align)
{
    if (QTableWidgetItem *h = t->horizontalHeaderItem(column))
        h->setTextAlignment(align | Qt::AlignVCenter);
}

// 批量填表前调用：先关掉列宽自适应，否则每填一格都会重算整列
inline void beginTableFill(QTableWidget *t)
{
    t->setUpdatesEnabled(false);
    for (int c = 0; c < t->columnCount(); ++c)
        t->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Interactive);
}

// 批量填表后调用：恢复自适应列宽并重绘
inline void endTableFill(QTableWidget *t, std::initializer_list<int> stretchCols = {})
{
    autoSizeColumns(t, stretchCols);
    t->setUpdatesEnabled(true);
}

inline QTableWidgetItem *item(const QString &text,
                              int align = Qt::AlignCenter,
                              const QColor &color = QColor())
{
    auto *it = new QTableWidgetItem(text);
    it->setTextAlignment(Qt::Alignment(align) | Qt::AlignVCenter);
    if (color.isValid())
        it->setForeground(color);
    return it;
}

inline QColor gold() { return QColor(QStringLiteral("#FDB927")); }
inline QColor dim() { return QColor(QStringLiteral("#8A97AC")); }
inline QColor red() { return QColor(QStringLiteral("#FF6B7A")); }
inline QColor green() { return QColor(QStringLiteral("#37D67A")); }

}  // namespace ui
