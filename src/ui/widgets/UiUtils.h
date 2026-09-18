#pragma once

#include <QColor>
#include <QHeaderView>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <initializer_list>

namespace ui {

// 统一初始化只读表格：整行单选、禁止编辑、交替行底色、隐藏网格线
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

// 列宽策略：列出的列拉伸占满剩余空间，其余列按内容自适应
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

// 批量填表前切换列宽模式并暂停重绘：若保持 ResizeToContents，每插入一格都会触发整列
// 重算，页面切换曾因此耗时约 1.3s，改为 Interactive 后降至 35ms 以下
inline void beginTableFill(QTableWidget *t)
{
    t->setUpdatesEnabled(false);
    for (int c = 0; c < t->columnCount(); ++c)
        t->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Interactive);
}

// 与 beginTableFill 成对调用：恢复列宽策略并重新开启重绘，使表格一次性完成布局
inline void endTableFill(QTableWidget *t, std::initializer_list<int> stretchCols = {})
{
    autoSizeColumns(t, stretchCols);
    t->setUpdatesEnabled(true);
}

// 构造表格单元并统一应用对齐与前景色；返回裸指针，所有权移交给表格接管
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
inline QColor green() { return QColor(QStringLiteral("#37D67A")); }

}  // namespace ui
