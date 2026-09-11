#pragma once

#include <QColor>
#include <QHeaderView>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <initializer_list>

// 页面通用小工具
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

// 所有列按内容自适应；stretchCols 中的列改为拉伸填充剩余空间
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

// 让表头的对齐方式与单元格保持一致（数值列居中、文本列左对齐）
inline void alignHeader(QTableWidget *t, int column, Qt::Alignment align)
{
    if (QTableWidgetItem *h = t->horizontalHeaderItem(column))
        h->setTextAlignment(align | Qt::AlignVCenter);
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
