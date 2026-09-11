#pragma once

#include <QColor>
#include <QHeaderView>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>

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
